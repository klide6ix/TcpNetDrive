// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "LLLNetDriver.h"
#include "LLLNetConnection.h"

#include "Misc/CommandLine.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/Package.h"

#include "SocketSubsystem.h"
#include "Sockets.h"
#include "IPAddress.h"

ULLLNetDriver::ULLLNetDriver(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool ULLLNetDriver::IsAvailable() const
{
	return true;
}

ISocketSubsystem* ULLLNetDriver::GetSocketSubsystem()
{
	//return ISocketSubsystem::Get(LLL_SUBSYSTEM);
	return ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
}

bool ULLLNetDriver::InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error)
{
	_LLLNetworkNotify._BaseNotifier = InNotify;

	// Skip UIpNetDriver implementation
	if (!UNetDriver::InitBase(bInitAsClient, &_LLLNetworkNotify, URL, bReuseAddressAndPort, Error))
	{
		return false;
	}

	// Success.
	return true;
}

bool ULLLNetDriver::InitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error)
{
	_LLLNetworkNotify._BaseNotifier = InNotify;

	// Skip UIpNetDriver implementation
	if (!UNetDriver::InitBase(true, InNotify, ConnectURL, false, Error))
	{
		UE_LOG(LogNet, Warning, TEXT("Failed to init net driver ConnectURL: %s: %s"), *ConnectURL.ToString(), *Error);
		return false;
	}

	InitConnectionlessHandler();

	ISocketSubsystem* SocketSubsystem = GetSocketSubsystem();
	if (SocketSubsystem == nullptr)
	{
		UE_LOG(LogNet, Error, TEXT("Failed to get SocketSubsystem ConnectURL: %s: %s"), *ConnectURL.ToString(), *Error);
		return false;
	}

	FSocket* Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("Unreal client (LLL)"), ESocketProtocolFamily::IPv4);
	if (Socket == nullptr)
	{
		UE_LOG(LogNet, Error, TEXT("Failed to create socket ConnectURL: %s: %s"), *ConnectURL.ToString(), *Error);
		return false;
	}

	// Create new connection.
	ServerConnection = NewObject<ULLLNetConnection>(GetTransientPackage(), NetConnectionClass);
	ServerConnection->InitLocalConnection(this, Socket, ConnectURL, USOCK_Pending);
	UE_LOG(LogNet, Log, TEXT("Game client on port %i, rate %i"), ConnectURL.Port, ServerConnection->CurrentNetSpeed);

	CreateInitialClientChannels();

	return true;
}

bool ULLLNetDriver::InitListen(FNetworkNotify* InNotify, FURL& ListenURL, bool bReuseAddressAndPort, FString& Error)
{
	_LLLNetworkNotify._BaseNotifier = InNotify;

	if (!InitBase(false, InNotify, ListenURL, bReuseAddressAndPort, Error))
	{
		return false;
	}

	InitConnectionlessHandler();

	ISocketSubsystem* SocketSubsystem = GetSocketSubsystem();
	if (SocketSubsystem == NULL)
	{
		UE_LOG(LogNet, Warning, TEXT("Unable to find socket subsystem"));
		Error = TEXT("Unable to find socket subsystem");
		return false;
	}

	_LLLSocketServer = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("Unreal client (LLL)"), ESocketProtocolFamily::IPv4);

	if (_LLLSocketServer == NULL)
	{
		Error = FString::Printf(TEXT("LLLSockets: socket failed (%i)"), (int32)SocketSubsystem->GetLastErrorCode());
		return false;
	}

	_LocalAddr = SocketSubsystem->GetLocalBindAddr(*GLog);
	_LocalAddr->SetPort(ListenURL.Port);

	_LLLSocketServer->Bind(*_LocalAddr);
	_LLLSocketServer->Listen(1024);

	// server has no server connection.
	ServerConnection = nullptr;
	return true;
}

void ULLLNetDriver::TickDispatch(float DeltaTime)
{
	UNetDriver::TickDispatch(DeltaTime);

	// Accept
	if (_LLLSocketServer != nullptr)
	{
		bool bHasPendingConnection = false;
		if (_LLLSocketServer->HasPendingConnection(bHasPendingConnection) == true && bHasPendingConnection == true)
		{
			// accept
			TSharedRef<FInternetAddr> Address = GetSocketSubsystem()->CreateInternetAddr();
			FString InSocketDescription;
			FSocket* sock = _LLLSocketServer->Accept(*Address, InSocketDescription);

			ULLLNetConnection* NewConnection = NewObject<ULLLNetConnection>(GetTransientPackage(), NetConnectionClass);
			check(NewConnection != nullptr);

			NewConnection->InitRemoteConnection(this, sock, World ? World->URL : FURL(), *Address, USOCK_Open);

			Notify->NotifyAcceptedConnection(NewConnection);
			AddClientConnection(NewConnection);

			NewConnection->bChallengeHandshake = true;
		}
	}

	// IO
	if (ServerConnection != nullptr)
	{
		ServerConnection->Tick();
	}
	else
	{
		for (UNetConnection* conn : ClientConnections)
		{
			conn->Tick();
		}
	}
}

bool ULLLNetDriver::IsNetResourceValid()
{
	return true;
}

void ULLLNetDriver::LowLevelSend(FString Address, void* Data, int32 CountBits, FOutPacketTraits& Traits)
{
	bool bValidAddress = !Address.IsEmpty();
	
	if (bValidAddress)
	{
		// TODO : Map 같은 걸로 잘 찾아보자.
		for (UNetConnection* conn : ClientConnections)
		{
			if (conn->LowLevelGetRemoteAddress(true) == Address)
			{
				conn->LowLevelSend(Data, CountBits, Traits);
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogNet, Warning, TEXT("ULLLNetDriver::LowLevelSend: Invalid send address '%s'"), *Address);
	}
}

FString ULLLNetDriver::LowLevelGetNetworkNumber()
{
	return _LocalAddr->ToString(true);
}

void ULLLNetDriver::LowLevelDestroy()
{
	Super::LowLevelDestroy();

	if (_LLLSocketServer != nullptr)
	{
		ISocketSubsystem* SocketSubsystem = GetSocketSubsystem();
		if (!_LLLSocketServer->Close())
		{
			UE_LOG(LogExit, Log, TEXT("closesocket error (%i)"), (int32)SocketSubsystem->GetLastErrorCode());
		}
		// Free the memory the OS allocated for this socket
		SocketSubsystem->DestroySocket(_LLLSocketServer);
		_LLLSocketServer = nullptr;
		UE_LOG(LogExit, Log, TEXT("%s shut down"), *GetDescription());
	}
}
