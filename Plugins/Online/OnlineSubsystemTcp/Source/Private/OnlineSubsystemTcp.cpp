// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OnlineSubsystemLLL.h"
#include "OnlineSubsystemLLLPCH.h"
#include "Engine/Console.h"
#include "Misc/ConfigCacheIni.h"

IOnlineSessionPtr FOnlineSubsystemLLL::GetSessionInterface() const
{
	return nullptr;
}

IOnlineFriendsPtr FOnlineSubsystemLLL::GetFriendsInterface() const
{
	return nullptr;
}

IOnlinePartyPtr FOnlineSubsystemLLL::GetPartyInterface() const
{
	return nullptr;
}

IOnlineGroupsPtr FOnlineSubsystemLLL::GetGroupsInterface() const
{
	return nullptr;
}

IOnlineSharedCloudPtr FOnlineSubsystemLLL::GetSharedCloudInterface() const
{
	return nullptr;
}

IOnlineUserCloudPtr FOnlineSubsystemLLL::GetUserCloudInterface() const
{
	return nullptr;
}

IOnlineEntitlementsPtr FOnlineSubsystemLLL::GetEntitlementsInterface() const
{
	return nullptr;
}

IOnlineLeaderboardsPtr FOnlineSubsystemLLL::GetLeaderboardsInterface() const
{
	return nullptr;
}

IOnlineVoicePtr FOnlineSubsystemLLL::GetVoiceInterface() const
{
	return nullptr;
}

IOnlineExternalUIPtr FOnlineSubsystemLLL::GetExternalUIInterface() const
{
	return nullptr;
}

IOnlineTimePtr FOnlineSubsystemLLL::GetTimeInterface() const
{
	return nullptr;
}

IOnlineIdentityPtr FOnlineSubsystemLLL::GetIdentityInterface() const
{
	return nullptr;
}

IOnlineTitleFilePtr FOnlineSubsystemLLL::GetTitleFileInterface() const
{
	return nullptr;
}

IOnlineStorePtr FOnlineSubsystemLLL::GetStoreInterface() const
{
	return nullptr;
}

IOnlineStoreV2Ptr FOnlineSubsystemLLL::GetStoreV2Interface() const
{
	return nullptr;
}

IOnlinePurchasePtr FOnlineSubsystemLLL::GetPurchaseInterface() const
{
	return nullptr;
}

IOnlineEventsPtr FOnlineSubsystemLLL::GetEventsInterface() const
{
	return nullptr;
}

IOnlineAchievementsPtr FOnlineSubsystemLLL::GetAchievementsInterface() const
{
	return nullptr;
}

IOnlineSharingPtr FOnlineSubsystemLLL::GetSharingInterface() const
{
	return nullptr;
}

IOnlineUserPtr FOnlineSubsystemLLL::GetUserInterface() const
{
	return nullptr;
}

IOnlineMessagePtr FOnlineSubsystemLLL::GetMessageInterface() const
{
	return nullptr;
}

IOnlinePresencePtr FOnlineSubsystemLLL::GetPresenceInterface() const
{
	return nullptr;
}

IOnlineChatPtr FOnlineSubsystemLLL::GetChatInterface() const
{
	return nullptr;
}

IOnlineStatsPtr FOnlineSubsystemLLL::GetStatsInterface() const
{
	return nullptr;
}

IOnlineTurnBasedPtr FOnlineSubsystemLLL::GetTurnBasedInterface() const
{
	return nullptr;
}

IOnlineTournamentPtr FOnlineSubsystemLLL::GetTournamentInterface() const
{
	return nullptr;
}

bool FOnlineSubsystemLLL::Init()
{
	UE_LOG_ONLINE(VeryVerbose, TEXT("FOnlineSubsystemLLL::Init() Name: %s"), *InstanceName.ToString());

	return true;
}

void FOnlineSubsystemLLL::PreUnload()
{
	UE_LOG_ONLINE(VeryVerbose, TEXT("FOnlineSubsystemLLL::Preunload() Name: %s"), *InstanceName.ToString());
}

bool FOnlineSubsystemLLL::Shutdown()
{
	UE_LOG_ONLINE(VeryVerbose, TEXT("FOnlineSubsystemLLL::Shutdown() Name: %s"), *InstanceName.ToString());

	FOnlineSubsystemImpl::Shutdown();

	return true;
}

FString FOnlineSubsystemLLL::GetAppId() const
{
	FString ClientId;
	if (!GConfig->GetString(TEXT("OnlineSubsystemLLL"), TEXT("ClientId"), ClientId, GEngineIni) ||
		ClientId.IsEmpty())
	{
		static bool bWarned = false;
		if (!bWarned)
		{
			bWarned = true;
			UE_LOG_ONLINE(Warning, TEXT("Missing ClientId= in [OnlineSubsystemLLL] of DefaultEngine.ini"));
		}
	}
	return ClientId;
}

bool FOnlineSubsystemLLL::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FOnlineSubsystemImpl::Exec(InWorld, Cmd, Ar))
	{
		return true;
	}

	return true;
}

FText FOnlineSubsystemLLL::GetOnlineServiceName() const
{
	return NSLOCTEXT("OnlineSubsystemLLL", "OnlineServiceName", "LLL");
}


FOnlineSubsystemLLL::FOnlineSubsystemLLL(FName InInstanceName)
	: FOnlineSubsystemImpl(LLL_SUBSYSTEM, InInstanceName)
{
}
