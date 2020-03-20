// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

//
// LLL sockets based implementation of the net driver
// 초기 버전 - UIpNetDriver 를 이용해 Stream 방식의 통신을 시도해 보고 필요에 따라서 UNetDriver를 이용한 재정의를 시도해본다.
//

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/NetDriver.h"
#include "OnlineSubsystemLLLNotify.h"
#include "LLLNetDriver.generated.h"

class Error;

UCLASS(transient, config=Engine)
class ULLLNetDriver : public UNetDriver
{
	GENERATED_UCLASS_BODY()

	//~ Begin UNetDriver Interface.
	virtual bool IsAvailable() const override;
	virtual bool InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error) override;
	virtual bool InitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error) override;
	virtual bool InitListen(FNetworkNotify* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error) override;
	virtual void TickDispatch(float DeltaTime) override;
	virtual void LowLevelSend(FString Address, void* Data, int32 CountBits, FOutPacketTraits& Traits) override;
	virtual FString LowLevelGetNetworkNumber() override;
	virtual void LowLevelDestroy() override;
	virtual class ISocketSubsystem* GetSocketSubsystem() override;
	virtual bool IsNetResourceValid(void) override;
	//~ End UNetDriver Interface

protected:
	TSharedPtr<FInternetAddr> _LocalAddr;

	FLLLNetworkNotify		_LLLNetworkNotify;
	FSocket*				_LLLSocketServer = nullptr;
};
