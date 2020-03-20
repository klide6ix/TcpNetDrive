// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OnlineSubsystemLLLPCH.h"

IMPLEMENT_MODULE(FOnlineSubsystemLLLModule, OnlineSubsystemLLL);

/**
 * Class responsible for creating instance(s) of the subsystem
 */
class FOnlineFactoryLLL : public IOnlineFactory
{
public:

	FOnlineFactoryLLL() = default;
	virtual ~FOnlineFactoryLLL() = default;

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName)
	{
		FOnlineSubsystemLLLPtr OnlineSub = MakeShared<FOnlineSubsystemLLL, ESPMode::ThreadSafe>(InstanceName);
		if (OnlineSub->IsEnabled())
		{
			if (!OnlineSub->Init())
			{
				UE_LOG_ONLINE(Warning, TEXT("LLL API failed to initialize instance %s!"), *InstanceName.ToString());
				OnlineSub->Shutdown();
				OnlineSub = nullptr;
			}

			return OnlineSub;
		}
		else
		{
			static bool bHasAlerted = false;
			if (!bHasAlerted)
			{
				// Alert once with a warning for visibility (should be at the beginning)
				UE_LOG_ONLINE(Log, TEXT("LLL API disabled."));
				bHasAlerted = true;
			}
			OnlineSub->Shutdown();
			OnlineSub.Reset();
		}

		return nullptr;
	}
};

void FOnlineSubsystemLLLModule::StartupModule()
{
	UE_LOG_ONLINE(Verbose, TEXT("LLL Startup!"));

	LLLFactory = MakeUnique<FOnlineFactoryLLL>();

	// Create and register our singleton factory with the main online subsystem for easy access
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.RegisterPlatformService(LLL_SUBSYSTEM, LLLFactory.Get());
}

void FOnlineSubsystemLLLModule::ShutdownModule()
{
	UE_LOG_ONLINE(Verbose, TEXT("LLL Shutdown!"));

	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.UnregisterPlatformService(LLL_SUBSYSTEM);

	LLLFactory.Reset();
}
