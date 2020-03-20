// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "LLLNetConnection.h"
#include "LLLNetDriver.h"

#include "Net/NetworkProfiler.h"
#include "Net/DataChannel.h"
#include "PacketHandlers/StatelessConnectHandlerComponent.h"

#include "OnlineSubsystemNames.h"
#include "OnlineSubsystem.h"
#include "IPAddress.h"
#include "SocketSubsystem.h"
#include "Sockets.h"

#include "PacketAudit.h"

/*-----------------------------------------------------------------------------
Declarations.
-----------------------------------------------------------------------------*/

// Size of a UDP header.
#define IP_HEADER_SIZE     (20)
#define UDP_HEADER_SIZE    (IP_HEADER_SIZE+8)
#define WINSOCK_MAX_PACKET (512)

ULLLNetConnection::ULLLNetConnection(const FObjectInitializer& ObjectInitializer) : UNetConnection(ObjectInitializer)
{
}

void ULLLNetConnection::InitBase(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket, int32 InPacketOverhead)
{
	// Pass the call up the chain
	Super::InitBase(InDriver, InSocket, InURL, InState,
		// Use the default packet size/overhead unless overridden by a child class
		InMaxPacket == 0 ? WINSOCK_MAX_PACKET : InMaxPacket,
		InPacketOverhead == 0 ? UDP_HEADER_SIZE : InPacketOverhead);

	Socket = InSocket;
	ResolveInfo = NULL;
}

void ULLLNetConnection::InitRemoteConnection(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, const class FInternetAddr& InRemoteAddr, EConnectionState InState, int32 InMaxPacket, int32 InPacketOverhead)
{
	InitBase(InDriver, InSocket, InURL, InState,
		// Use the default packet size/overhead unless overridden by a child class
		(InMaxPacket == 0 || InMaxPacket > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : InMaxPacket,
		InPacketOverhead == 0 ? UDP_HEADER_SIZE : InPacketOverhead);

	// Copy the remote IPAddress passed in
	bool bIsValid = false;
	FString IpAddrStr = InRemoteAddr.ToString(false);
	RemoteAddr = InDriver->GetSocketSubsystem()->CreateInternetAddr();
	RemoteAddr->SetIp(*IpAddrStr, bIsValid);
	RemoteAddr->SetPort(InRemoteAddr.GetPort());

	URL.Host = RemoteAddr->ToString(false);

	// Initialize our send bunch
	InitSendBuffer();

	// This is for a client that needs to log in, setup ClientLoginState and ExpectedClientLoginMsgType to reflect that
	SetClientLoginState(EClientLoginState::LoggingIn);
	SetExpectedClientLoginMsgType(NMT_Hello);
}

void ULLLNetConnection::InitLocalConnection(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket, int32 InPacketOverhead)
{
	InitBase(InDriver, InSocket, InURL, InState,
		// Use the default packet size/overhead unless overridden by a child class
		(InMaxPacket == 0 || InMaxPacket > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : InMaxPacket, 0);

	// Figure out IP address from the host URL
	bool bIsValid = false;
	// Get numerical address directly.
	RemoteAddr = InDriver->GetSocketSubsystem()->CreateInternetAddr();
	RemoteAddr->SetIp(*InURL.Host, bIsValid);
	RemoteAddr->SetPort(InURL.Port);

	// Try to resolve it if it failed
	if (bIsValid == false)
	{
		// Create thread to resolve the address.
		ResolveInfo = InDriver->GetSocketSubsystem()->GetHostByName(TCHAR_TO_ANSI(*InURL.Host));
		if (ResolveInfo == NULL)
		{
			Close();
			UE_LOG(LogNet, Verbose, TEXT("IpConnection::InitConnection: Unable to resolve %s"), *InURL.Host);
		}
	}

	// Initialize our send bunch
	InitSendBuffer();

	if (InSocket->Connect(*GetRemoteAddr()) == false)
	{
		UE_LOG(LogNet, Error, TEXT("Failed to connect net driver ConnectURL: %s"), *InURL.ToString());
	}
}

void ULLLNetConnection::LowLevelSend(void* Data, int32 CountBits, FOutPacketTraits& Traits)
{
	const uint8* DataToSend = reinterpret_cast<uint8*>(Data);

	// Process any packet modifiers
	if (Handler.IsValid() && !Handler->GetRawSend())
	{
		const ProcessedPacket ProcessedData = Handler->Outgoing(reinterpret_cast<uint8*>(Data), CountBits, Traits);

		if (!ProcessedData.bError)
		{
			DataToSend = ProcessedData.Data;
			CountBits = ProcessedData.CountBits;
		}
		else
		{
			CountBits = 0;
		}
	}

	bool bBlockSend = false;
	int32 CountBytes = FMath::DivideAndRoundUp(CountBits, 8);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogNet, Warning, TEXT("ULLLNetConnection::LowLevelSend: Address: %s, CountBytes: %i"), *LowLevelGetRemoteAddress(true), CountBytes);

	LowLevelSendDel.ExecuteIfBound((void*)DataToSend, CountBytes, bBlockSend);
#endif
	int32 BytesSent = 0;
	if (!bBlockSend && CountBytes > 0)
	{
		Socket->Send((uint8*)DataToSend, CountBytes, BytesSent);
	}
}

FString ULLLNetConnection::LowLevelGetRemoteAddress(bool bAppendPort)
{
	return RemoteAddr->ToString(bAppendPort);
}

FString ULLLNetConnection::LowLevelDescribe()
{
	TSharedRef<FInternetAddr> LocalAddr = Driver->GetSocketSubsystem()->CreateInternetAddr();

	if (Socket != nullptr)
	{
		Socket->GetAddress(*LocalAddr);
	}

	return FString::Printf
	(
		TEXT("url=%s remote=%s local=%s uniqueid=%s state: %s"),
		*URL.Host,
		(RemoteAddr.IsValid() ? *RemoteAddr->ToString(true) : TEXT("nullptr")),
		*LocalAddr->ToString(true),
		(PlayerId.IsValid() ? *PlayerId->ToDebugString() : TEXT("nullptr")),
		State == USOCK_Pending ? TEXT("Pending")
		: State == USOCK_Open ? TEXT("Open")
		: State == USOCK_Closed ? TEXT("Closed")
		: TEXT("Invalid")
	);
}

int32 ULLLNetConnection::GetAddrPort(void)
{
	return RemoteAddr->GetPort();
}

FString ULLLNetConnection::RemoteAddressToString()
{
	return RemoteAddr->ToString(true);
}

void ULLLNetConnection::Tick()
{
	if (Socket != nullptr)
	{
		uint32 PendingDataSize = 0;
		if (Socket->HasPendingData(PendingDataSize) == true && PendingDataSize)
		{
			uint8 Data[MAX_PACKET_SIZE];
			int32 ReadBytes = 0;
			if (Socket->Recv(Data, MAX_PACKET_SIZE, ReadBytes) == true)
			{
				FPacketAudit::NotifyLowLevelReceive(Data, ReadBytes);

				ReceivedRawPacket(Data, ReadBytes);
			}
		}
	}

	Super::Tick();
}

void ULLLNetConnection::CleanUp()
{
	Super::CleanUp();
}

void ULLLNetConnection::ReceivedRawPacket(void* Data, int32 Count)
{
	if (Count == 0 ||   // nothing to process
		Driver == NULL) // connection closing
	{
		return;
	}

#if !UE_BUILD_SHIPPING
	UE_LOG(LogNet, Warning, TEXT("ULLLNetConnection::ReceivedRawPacket: Address: %s, Count: %i"), *LowLevelGetRemoteAddress(true), Count);
#endif

	uint8* DataRef = reinterpret_cast<uint8*>(Data);
	if (bChallengeHandshake)
	{
		// Process all incoming packets.
		if (Driver->ConnectionlessHandler.IsValid() && Driver->StatelessConnectComponent.IsValid())
		{
			const ProcessedPacket UnProcessedPacket = Driver->ConnectionlessHandler->IncomingConnectionless(LowLevelGetRemoteAddress(true), DataRef, Count);

			TSharedPtr<StatelessConnectHandlerComponent> StatelessConnect = Driver->StatelessConnectComponent.Pin();

			bool bRestartedHandshake = false;
			bool bHasPassedChallenge = StatelessConnect->HasPassedChallenge(LowLevelGetRemoteAddress(true), bRestartedHandshake);

			if (!UnProcessedPacket.bError && !bHasPassedChallenge && !bRestartedHandshake)
			{
				UE_LOG(LogNet, Log, TEXT("Server accepting post-challenge connection from: %s"), *LowLevelGetRemoteAddress(true));
				// Set the initial packet sequence from the handshake data
				if (StatelessConnectComponent.IsValid())
				{
					int32 ServerSequence = 0;
					int32 ClientSequence = 0;
					StatelessConnect->GetChallengeSequence(ServerSequence, ClientSequence);
					InitSequence(ClientSequence, ServerSequence);
				}

				if (Handler.IsValid())
				{
					Handler->BeginHandshaking();
				}

				bChallengeHandshake = false; // i.e. bPassedChallenge
				UE_LOG(LogNet, Warning, TEXT("ULLLNetConnection::bChallengeHandshake: %s"), *LowLevelDescribe());
				Count = FMath::DivideAndRoundUp(UnProcessedPacket.CountBits, 8);
				if (Count > 0)
				{
					DataRef = UnProcessedPacket.Data;
				}
				else
				{
					return; // NO FURTHER DATA TO PROCESS
				}
			}
			else
			{
				// WARNING: if here, it might be during (bInitialConnect) - which needs to be processed (ReceivedRawPacket)
				//return;
			}
		}
	}

	UNetConnection::ReceivedRawPacket(DataRef, Count);
}