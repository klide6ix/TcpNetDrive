// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OnlineSubsystem.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemLLLModule.h"
#include "OnlineSubsystemLLLPackage.h"

/** Forward declarations of all interface classes */
typedef TSharedPtr<class FOnlineIdentityLLL, ESPMode::ThreadSafe> FOnlineIdentityLLLPtr;
typedef TSharedPtr<class FOnlineExternalUILLL, ESPMode::ThreadSafe> FOnlineExternalUILLLPtr;

class FOnlineAsyncTaskManagerLLL;
class FRunnableThread;

/**
 * LLL backend services
 */
class ONLINESUBSYSTEMLLL_API FOnlineSubsystemLLL : public FOnlineSubsystemImpl
{
public:

	// IOnlineSubsystem

	virtual IOnlineSessionPtr GetSessionInterface() const override;
	virtual IOnlineFriendsPtr GetFriendsInterface() const override;
	virtual IOnlinePartyPtr GetPartyInterface() const override;
	virtual IOnlineGroupsPtr GetGroupsInterface() const override;
	virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override;
	virtual IOnlineUserCloudPtr GetUserCloudInterface() const override;
	virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
	virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override;
	virtual IOnlineVoicePtr GetVoiceInterface() const override;
	virtual IOnlineExternalUIPtr GetExternalUIInterface() const override;
	virtual IOnlineTimePtr GetTimeInterface() const override;
	virtual IOnlineIdentityPtr GetIdentityInterface() const override;
	virtual IOnlineTitleFilePtr GetTitleFileInterface() const override;
	virtual IOnlineStorePtr GetStoreInterface() const override;
	virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override;
	virtual IOnlinePurchasePtr GetPurchaseInterface() const override;
	virtual IOnlineEventsPtr GetEventsInterface() const override;
	virtual IOnlineAchievementsPtr GetAchievementsInterface() const override;
	virtual IOnlineSharingPtr GetSharingInterface() const override;
	virtual IOnlineUserPtr GetUserInterface() const override;
	virtual IOnlineMessagePtr GetMessageInterface() const override;
	virtual IOnlinePresencePtr GetPresenceInterface() const override;
	virtual IOnlineChatPtr GetChatInterface() const override;
	virtual IOnlineStatsPtr GetStatsInterface() const override;
	virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override;
	virtual IOnlineTournamentPtr GetTournamentInterface() const override;
	virtual bool Init() override;
	virtual void PreUnload() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual FText GetOnlineServiceName() const override;

	// FOnlineSubsystemLLL

	/**
	 * Destructor
	 */
	virtual ~FOnlineSubsystemLLL() = default;

PACKAGE_SCOPE:

	/** Only the factory makes instances */
	explicit FOnlineSubsystemLLL(FName InInstanceName);

	/** Default constructor unavailable */
	FOnlineSubsystemLLL() = delete;

};

typedef TSharedPtr<FOnlineSubsystemLLL, ESPMode::ThreadSafe> FOnlineSubsystemLLLPtr;

#ifndef LLL_SUBSYSTEM
#define LLL_SUBSYSTEM FName(TEXT("LLL"))
#endif
