// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/NewGameMode/PartyManager/PartyManager.h"
#include "ClassicCarrouselPartyManager.generated.h"

UCLASS()
class AClassicCarrouselPartyManager : public APartyManager
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) TMap<FString, int> pointPerPlayer;

	UPROPERTY(EditAnywhere) TArray<FVector> carrouselSpawnPoint;
	UPROPERTY(EditAnywhere) int countObjectTake = 0;

	UPROPERTY(EditAnywhere) float timeForLastPlayer = 2.0f;
	UPROPERTY(EditAnywhere) float timeForMidPlayer = 4.0f;
	UPROPERTY(EditAnywhere) float timeForFirstPlayer = 6.0f;
	FTimerHandle timerFreeLastPlayer;
	FTimerHandle timerFreeMidPlayer;
	FTimerHandle timerFreeFirstPlayer;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<APlayerCharacter>> firstPlayers;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<APlayerCharacter>> midPlayers;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<APlayerCharacter>> lastPlayers;

public:	
	AClassicCarrouselPartyManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void InitParty(const TArray<TObjectPtr<APlayerCharacter>>& _players) override;
	virtual void StartParty()override;
	virtual void EndParty()override;

	void TriPlayerPerPoint();
	void FreeLastPlayer();
	void FreeMidPlayer();
	void FreeFirstPlayer();
	UFUNCTION() void ObjectTake(APlayerCharacter* _player, FItem _item);

	void SwitchToBattle();

};
