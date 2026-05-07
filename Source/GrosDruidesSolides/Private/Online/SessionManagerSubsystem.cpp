// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionManagerSubsystem.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineIdentityInterface.h"

void USessionManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//Get the online subsystem (steam or null)
	IOnlineSubsystem* _sys = IOnlineSubsystem::Get();

	if (!_sys)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find OnlineSubsystem !"));
		return;
	}
	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &USessionManagerSubsystem::OnNetworkFailure);
	}
	sessionInterface = _sys->GetSessionInterface();
	if (sessionInterface.IsValid())
	{
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USessionManagerSubsystem::OnCreateSessionComplete_Internal);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USessionManagerSubsystem::OnFindSessionsComplete_Internal);
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USessionManagerSubsystem::OnJoinSessionComplete_Internal);
		sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &USessionManagerSubsystem::OnDestroySessionComplete_Internal);
		sessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &USessionManagerSubsystem::OnInviteAccepted);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Session Interface !"));
	}
}

void USessionManagerSubsystem::Deinitialize()
{
	//Free/Clear delegates
	if (sessionInterface.IsValid())
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegates(this);
		sessionInterface->ClearOnFindSessionsCompleteDelegates(this);
		sessionInterface->ClearOnJoinSessionCompleteDelegates(this);
		sessionInterface->ClearOnDestroySessionCompleteDelegates(this);
		sessionInterface->ClearOnSessionUserInviteAcceptedDelegates(this);
	}
	sessionInterface = nullptr;
	sessionSearch = nullptr;

	Super::Deinitialize();
}

/*void USessionManagerSubsystem::CreateSession(int32 _maxPlayers, const FString& _serverName)
{
	if (!sessionInterface.IsValid())
	{
		OnCreateSessionComplete.Broadcast(false);
		return;
	}

	//save params for tentative restart
	savedServerName = _serverName;
	savedMaxPlayers = _maxPlayers;

	//Destroy existing session if needed
	FNamedOnlineSession* _existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if (_existingSession)
		sessionInterface->DestroySession(NAME_GameSession);

	IOnlineSubsystem* _oss = IOnlineSubsystem::Get();
	//UE_LOG(LogTemp, Warning, TEXT("Online Subsystem : %s"), *_oss->GetSubsystemName().ToString());
	UKismetSystemLibrary::PrintString(this, _oss->GetSubsystemName().ToString());
	//Create session's settings
	TSharedPtr<FOnlineSessionSettings> _sessionSettings = MakeShared<FOnlineSessionSettings>();
	//_sessionSettings->bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL"); //Keep Null for lan only
	_sessionSettings->bIsLANMatch = (_oss->GetSubsystemName() == "NULL"); //Keep Null for lan only
	_sessionSettings->bUsesPresence = true; //Required for steam
	_sessionSettings->bAllowJoinViaPresence = true;
	_sessionSettings->bAllowJoinInProgress = true;
	_sessionSettings->bShouldAdvertise = true;
	_sessionSettings->bUseLobbiesIfAvailable = true; //required for steam
	_sessionSettings->NumPublicConnections = _maxPlayers;
	//_sessionSettings->bIsDedicated = false;
	_sessionSettings->Set(FName("SERVER_NAME"), _serverName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	_sessionSettings->Set(FName("GAME_NAME"), FString("O3DGame"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//Create the session
	sessionInterface->CreateSession(0, NAME_GameSession, *_sessionSettings);
	UKismetSystemLibrary::PrintString(this, "Session Creation");

}*/

void USessionManagerSubsystem::CreateSession(int32 _maxPlayers, const FString& _serverName)
{
	if (!sessionInterface.IsValid())
	{
		OnCreateSessionComplete.Broadcast(false);
		return;
	}

	// save params for 
	savedServerName = _serverName;
	savedMaxPlayers = _maxPlayers;

	//Destroy existing session if needed
	FNamedOnlineSession* _existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if (_existingSession)
		sessionInterface->DestroySession(NAME_GameSession);

	// Create Session's settings
	TSharedPtr<FOnlineSessionSettings> _sessionSettings = MakeShared<FOnlineSessionSettings>();
	_sessionSettings->bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	_sessionSettings->bUsesPresence = true; //Required for steam
	_sessionSettings->NumPublicConnections = _maxPlayers;
	_sessionSettings->bAllowJoinViaPresence = true;
	_sessionSettings->bShouldAdvertise = true;
	_sessionSettings->bIsDedicated = false;
	_sessionSettings->bUseLobbiesIfAvailable = true; //required for steam
	_sessionSettings->bAllowJoinInProgress = true;
	_sessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	//_sessionSettings->bAllowJoinInProgress = false;
	_sessionSettings->bAllowInvites = true;
	_sessionSettings->Set(FName("SERVER_NAME"), _serverName+"ix", EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	_sessionSettings->Set(FName("GAME_NAME"), FString("O3DGame"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// Create the session
	sessionInterface->CreateSession(0, NAME_GameSession, *_sessionSettings);
	UKismetSystemLibrary::PrintString(this, "Session Create Started");
}

void USessionManagerSubsystem::FindSessions()
{
	if (!sessionInterface.IsValid())
	{
		OnFindSessionComplete.Broadcast(false);
		return;
	}

	sessionSearch = MakeShared<FOnlineSessionSearch>();
	IOnlineSubsystem* _oss = IOnlineSubsystem::Get();
	FString Msg = FString::Printf(TEXT("OSS: %s"), *_oss->GetSubsystemName().ToString());
	UKismetSystemLibrary::PrintString(this, Msg);
	//sessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	sessionSearch->bIsLanQuery = (_oss->GetSubsystemName() == "NULL");
	sessionSearch->MaxSearchResults = 10000;
	sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	sessionSearch->QuerySettings.Set(FName("GAME_NAME"), FString("O3DGame"), EOnlineComparisonOp::Equals);


	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
	UKismetSystemLibrary::PrintString(this, "Find Sessions", true, true, FLinearColor::Red);
}

void USessionManagerSubsystem::JoinSessionByIndex(int32 _sessionIndex)
{
	if (!sessionInterface || !sessionSearch.IsValid() || !sessionSearch->SearchResults.IsValidIndex(_sessionIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("Session interface invalid"));
		OnJoinSessionComplete.Broadcast(false);
		return;
	}
	FNamedOnlineSession* _existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if (_existingSession)
	{
		sessionInterface->DestroySession(NAME_GameSession);
		OnJoinSessionComplete.Broadcast(false);
		return;
	}
	UKismetSystemLibrary::PrintString(this, "Try Joining Session");

	sessionInterface->JoinSession(*IOnlineSubsystem::Get()->GetIdentityInterface()->GetUniquePlayerId(0), NAME_GameSession, sessionSearch->SearchResults[_sessionIndex]);

}

void USessionManagerSubsystem::DestroyCurrentSession()
{
	if (!sessionInterface.IsValid())
	{
		OnDestroySessionComplete.Broadcast();
		return;
	}
	sessionInterface->DestroySession(NAME_GameSession);
}

void USessionManagerSubsystem::OnNetworkFailure(UWorld* _world, UNetDriver* _netDriver, ENetworkFailure::Type _failureType, const FString& _errorString)
{
	UE_LOG(LogTemp, Warning, TEXT("Network failure : %s"), *_errorString);

	//Important : Cleanup client's session
	if (!sessionInterface.IsValid())return;
	FNamedOnlineSession* _existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if (_existingSession)
		sessionInterface->DestroySession(NAME_GameSession);

	//Optional : return to menu
	UGameplayStatics::OpenLevel(this, "LVL_MainMenu");
}


/* Internal Callbacks */

void USessionManagerSubsystem::OnCreateSessionComplete_Internal(FName _sessionName, bool _wasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("OnCreateSessionComplete : %s"), _wasSuccessful ? TEXT("Success") : TEXT("Fail"));
	OnCreateSessionComplete.Broadcast(_wasSuccessful);

	//If success, launch travel to host's level (optional)
	if (_wasSuccessful && GEngine)
	{
		UKismetSystemLibrary::PrintString(this, "Session Created Successfully");
		UWorld* _world = GetWorld();
		if (_world)
		{
			if (_world->GetNetMode() == NM_Client)
			{
				UKismetSystemLibrary::PrintString(this, "This is a client");
				return;
			}
			UKismetSystemLibrary::PrintString(this, "Traveling to world !");
			pendingLevel = "/Game/Level/LVL_Lobby"; //To Change;
			_world->ServerTravel(pendingLevel + "?listen");

			return;
		}
	}
	UKismetSystemLibrary::PrintString(this, "Session Creation Failed");
}

void USessionManagerSubsystem::OnFindSessionsComplete_Internal(bool _wasSuccessful)
{
	foundServers.Empty();
	UKismetSystemLibrary::PrintString(
		this,
		FString::Printf(TEXT("Search success: %s"), _wasSuccessful ? TEXT("Success") : TEXT("Failed"))
	);
	if (_wasSuccessful && sessionSearch.IsValid())
	{
		int32 _size = sessionSearch->SearchResults.Num();
		UE_LOG(LogTemp, Warning, TEXT("Search Result : %d"), sessionSearch->SearchResults.Num());
		for (int32 Index = 0; Index < _size; Index++)
		{
			const FOnlineSessionSearchResult& _result = sessionSearch->SearchResults[Index];

			FServerInfo _info;
			FString _name;
			if (_result.Session.SessionSettings.Get(FName("SERVER_NAME"), _name))
				_info.serverName = _name;
			else
			{
				_info.serverName = "Unknown";
			}

			_info.maxPlayers = _result.Session.SessionSettings.NumPublicConnections;
			_info.currentPlayers = _info.maxPlayers - _result.Session.NumOpenPublicConnections;
			_info.arrayIndex = Index;
			_info.ping = _result.PingInMs;

			foundServers.Add(_info);
		}
	}
	if (foundServers.Num() > 0)
		UKismetSystemLibrary::PrintString(this, "Find Sessions Successful", true, true, FLinearColor::Green);
	else
	{
		UKismetSystemLibrary::PrintString(this, "Find Sessions Failed", true, true, FLinearColor::Red);

	}
	OnFindSessionComplete.Broadcast(_wasSuccessful);
}

void USessionManagerSubsystem::OnJoinSessionComplete_Internal(FName _sessionName, EOnJoinSessionCompleteResult::Type _result)
{
	bool _success = (_result == EOnJoinSessionCompleteResult::Success);
	OnJoinSessionComplete.Broadcast(_success);
	UKismetSystemLibrary::PrintString(this, "Join Session Complete First");

	if (_success)
	{

		//Client travel
		UKismetSystemLibrary::PrintString(this, "Join Session Complete Success");
		APlayerController* _pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (_pc && sessionInterface.IsValid())
		{
			FString _connectString;
			if (sessionInterface->GetResolvedConnectString(_sessionName, _connectString))
			{
				UKismetSystemLibrary::PrintString(this, "Connecting to: " + _connectString, true, true, FLinearColor::Yellow);
				_pc->ClientTravel(_connectString, TRAVEL_Absolute);
			}
		}

	}
	else
	{
		UKismetSystemLibrary::PrintString(this, "Try Joining Session Failed");
	}
}

void USessionManagerSubsystem::OnDestroySessionComplete_Internal(FName _sessionName, bool _wasSuccessul)
{
	UE_LOG(LogTemp, Log, TEXT("Session Destroyed (client cleanup"));
	sessionSearch = nullptr;
	foundServers.Empty();

	UE_LOG(LogTemp, Log, TEXT("OnDestroyComplete : %s"), _wasSuccessul ? TEXT("Success") : TEXT("Fail"));
	OnDestroySessionComplete.Broadcast();
}

void USessionManagerSubsystem::OnInviteAccepted(bool _wasSuccessful, int32 _localUserNum, TSharedPtr<const FUniqueNetId> _userId, const FOnlineSessionSearchResult& _inviteResult)
{
	if (!_wasSuccessful || !sessionInterface.IsValid()) return;

	FNamedOnlineSession* _existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if (_existingSession)
	{
		sessionInterface->DestroySession(NAME_GameSession);
		return;
	}

	sessionInterface->JoinSession(
		*IOnlineSubsystem::Get()->GetIdentityInterface()->GetUniquePlayerId(0),
		NAME_GameSession,
		_inviteResult);
}
