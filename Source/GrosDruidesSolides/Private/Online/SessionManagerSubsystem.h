// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

//Online
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

#include "SessionManagerSubsystem.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateSessionCompleteBP, bool, wasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFindSessionCompleteBP, bool, wasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinSessionCompleteBP, bool, wasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroySessionCompleteBP);


USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString serverName = "Server";

	UPROPERTY(BlueprintReadOnly)
	int32 currentPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 maxPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ping = 0;

	//Index interne pour rejoindre la session
	UPROPERTY(BlueprintReadOnly)
	int32 arrayIndex = -1;

};


UCLASS()
class GROSDRUIDESSOLIDES_API USessionManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/*Blueprint Events*/
	UPROPERTY(BlueprintAssignable, Category = "Session")
	FOnCreateSessionCompleteBP OnCreateSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Session")
	FOnFindSessionCompleteBP OnFindSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Session")
	FOnJoinSessionCompleteBP OnJoinSessionComplete;

	UPROPERTY(BlueprintAssignable, Category = "Session")
	FOnDestroySessionCompleteBP OnDestroySessionComplete;


	/* Blueprint Data */
	UPROPERTY(BlueprintReadOnly, Category = "Session")
	TArray<FServerInfo> foundServers;

	FString pendingLevel = "";

	/* Subystsem Life Cycle */
	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize()override;


	UFUNCTION(BlueprintCallable, Category = "Session")
	void CreateSession(int32 _maxPlayers, const FString& _serverName);

	UFUNCTION(BlueprintCallable, Category = "Session")
	void FindSessions();

	UFUNCTION(BlueprintCallable, Category = "Session")
	void JoinSessionByIndex(int32 _sessionIndex);

	UFUNCTION(BlueprintCallable, Category = "Session")
	void DestroyCurrentSession();

	void OnNetworkFailure(UWorld* _world, UNetDriver* _netDriver, ENetworkFailure::Type _failureType, const FString& _errorString);
private:

	/* Online - Internal, no need for uproperty */

	IOnlineSessionPtr sessionInterface;
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	/* Saved params */
	FString savedServerName = "";
	int32 savedMaxPlayers = 0;

	/* Internal Callbacks */
	void OnCreateSessionComplete_Internal(FName _sessionName, bool _wasSuccessful);
	void OnFindSessionsComplete_Internal(bool _wasSuccessful);
	void OnJoinSessionComplete_Internal(FName _sessionName, EOnJoinSessionCompleteResult::Type _result);
	void OnDestroySessionComplete_Internal(FName _sessionName, bool _wasSuccessul);
	void OnInviteAccepted(bool _wasSuccessful, int32 _localUserNum, TSharedPtr<const FUniqueNetId> _userId, const FOnlineSessionSearchResult& _inviteResult);

};