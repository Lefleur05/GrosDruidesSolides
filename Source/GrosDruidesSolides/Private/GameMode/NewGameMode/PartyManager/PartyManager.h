// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInstance/UPartyGameInstanceSubsystem.h"
#include "PartyManager.generated.h"

class AAGDSGameMode;
class APlayerCharacter;

UCLASS(Abstract)
class APartyManager : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Replicated) TArray<TObjectPtr<APlayerCharacter>> allPlayers;
	UPROPERTY(EditAnywhere) EPartyType partyType = EPartyType::PT_NONE;
	UPROPERTY(EditAnywhere) TObjectPtr<AAGDSGameMode> gameMode;

	UPROPERTY(EditAnywhere) float timeBeforeSwitch = 5.0f;
	FTimerHandle timerSwitchLevel;

	UPROPERTY(EditAnywhere) bool pieAllPlayerSpawn = false;

	UPROPERTY(EditAnywhere) TSubclassOf<UUserWidget> uiForLevel = nullptr;

public:
	EPartyType GetPartyType() const { return partyType; }

	void SetGameMode(TObjectPtr<AAGDSGameMode> _gameMode) { gameMode = _gameMode; }

public:	
	APartyManager();

public:
	virtual void BeginPlay() override;
 	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitParty(const TArray<TObjectPtr<APlayerCharacter>>& _players);
	virtual void StartParty() PURE_VIRTUAL(APartyManager::StartParty, );
	virtual void EndParty() {}
	void SpawnUI();

};
