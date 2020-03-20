// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

//
// LLL sockets based implementation of a network connection used by the LLL net driver class
//

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "IpConnection.h"
#include "LLLNetConnection.generated.h"

UCLASS(transient, config=Engine)
class ULLLNetConnection : public UNetConnection
{
    GENERATED_UCLASS_BODY()

	//~ Begin UIpConnection Interface
	TSharedPtr<FInternetAddr>	RemoteAddr;

	class FSocket*				Socket;
	class FResolveInfo*			ResolveInfo;

	//~ Begin NetConnection Interface
	virtual void InitBase(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override;
	virtual void InitRemoteConnection(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, const class FInternetAddr& InRemoteAddr, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override;
	virtual void InitLocalConnection(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override;
	virtual void LowLevelSend(void* Data, int32 CountBits, FOutPacketTraits& Traits) override;
	FString LowLevelGetRemoteAddress(bool bAppendPort = false) override;
	FString LowLevelDescribe() override;
	virtual int32 GetAddrPort(void) override;
	virtual FString RemoteAddressToString() override;
	virtual void Tick() override;
	virtual void CleanUp() override;
	virtual void ReceivedRawPacket(void* Data, int32 Count);
	
	virtual TSharedPtr<FInternetAddr> GetInternetAddr() { return ConstCastSharedPtr<FInternetAddr>(GetRemoteAddr()); }
	virtual TSharedPtr<const FInternetAddr> GetRemoteAddr() { return RemoteAddr; }
	//~ End NetConnection Interface

	bool bChallengeHandshake = false;
};
