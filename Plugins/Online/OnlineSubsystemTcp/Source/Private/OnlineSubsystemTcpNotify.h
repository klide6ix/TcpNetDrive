// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/NetworkDelegates.h"

class FLLLNetworkNotify : public FNetworkNotify
{
public:
	virtual EAcceptConnection::Type NotifyAcceptingConnection() override;
	virtual void NotifyAcceptedConnection(class UNetConnection* Connection) override;
	virtual bool NotifyAcceptingChannel(class UChannel* Channel) override;
	virtual void NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch) override;

private:
	FNetworkNotify* _BaseNotifier = nullptr;

	friend class ULLLNetDriver;
};