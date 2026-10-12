// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameMode/PlayerState/InfoPlayerState.h"
#include "GameInstance/UPartyGameInstanceSubsystem.h"
#include "AGDSGameMode.generated.h"


class AMultyplayerCamera;
class APlayerCharacter;
class APartyManager;
class AMapConfig;

/**
 * 
 */
UCLASS()
class AAGDSGameMode : public AGameMode
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere) bool allPlayerJoinSession = false;
	UPROPERTY(EditAnywhere) int playerJoin = 0;

	UPROPERTY(EditAnywhere) TArray<TObjectPtr<APlayerCharacter>> allPlayers = TArray<TObjectPtr<APlayerCharacter>>();
	UPROPERTY(EditAnywhere) TObjectPtr<AMapConfig> mapConfig;
	UPROPERTY(EditAnywhere) TObjectPtr<AMultyplayerCamera> currentCamera;
	UPROPERTY(EditAnywhere) TMap<EPlayerColor, TObjectPtr<AInfoPlayerState>> playerColor;


	UPROPERTY(EditAnywhere) TMap<EPartyType, TSubclassOf<APartyManager>> allPartyType;
	UPROPERTY(EditAnywhere)	TObjectPtr<APartyManager> currentPartyType;

	UPROPERTY(EditAnywhere)	TSubclassOf<APlayerCharacter> playerCharacterClass;
public:

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void PostSeamlessTravel() override;
	virtual void Logout(AController* Exiting) override;
	UFUNCTION() void RequestRespawn(AController* _controller);
	virtual void NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player);
	void UpdateCameraWithNewPlayer(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player);


	//Utilitaire
	void SetPlayerLocationAndRotation();
	void StartCountDown(float _countDown, bool _countDownImmediatlyStart = true);
	void SelectPlayerColor(TObjectPtr <APlayerController> _newPlayer);

	void SpawnPartyManager();
	TArray<TObjectPtr<APlayerCharacter>> SpawnAllPlayers(const TArray<TObjectPtr<APlayerController>>& _controllers);
	
};
