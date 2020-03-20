// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OnlineSubsystemLLLNotify.h"

EAcceptConnection::Type FLLLNetworkNotify::NotifyAcceptingConnection()
{
	if (_BaseNotifier != nullptr)
	{
		return _BaseNotifier->NotifyAcceptingConnection();
	}

	return EAcceptConnection::Reject;
}

void FLLLNetworkNotify::NotifyAcceptedConnection(class UNetConnection* Connection)
{
	if (_BaseNotifier != nullptr)
	{
		_BaseNotifier->NotifyAcceptedConnection(Connection);
	}
}

bool FLLLNetworkNotify::NotifyAcceptingChannel(class UChannel* Channel)
{
	if (_BaseNotifier != nullptr)
	{
		return _BaseNotifier->NotifyAcceptingChannel(Channel);
	}

	return false;
}

void FLLLNetworkNotify::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch)
{
	if (_BaseNotifier != nullptr)
	{
		_BaseNotifier->NotifyControlMessage(Connection, MessageType, Bunch);
	}
}