// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "InfoPlayerGameInstance.generated.h"

#define GA_GAME_KEY "0cb20cc20cd456fc8a409f0e9047d064"
#define GA_SECRET_KEY "5424a77773f94792b9567d6fd5bc580775240950"

USTRUCT()
struct FLevelInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)FString map;
	UPROPERTY(EditAnywhere)TArray<FVector> allSpawnPoint;

	FLevelInfo()
	{
		map = FString();
		allSpawnPoint = TArray<FVector>();
	}

	FLevelInfo(FString _map, TArray<FVector> _allSpawnPoint = TArray<FVector>())
	{
		map = _map;
		allSpawnPoint = _allSpawnPoint;
	}
};

UCLASS()
class UInfoPlayerGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) int lobbyPlayerCount = 0;

	UPROPERTY(EditAnywhere)FLevelInfo carrouselMap = FLevelInfo("SLVL_CarrouselOnline");
	UPROPERTY(EditAnywhere)TArray<FLevelInfo> allBattleMap;

	UPROPERTY(EditAnywhere)TMap<FString,int> pointPerPlayer;

public:
	FORCEINLINE void SetLobbyPlayerCount(int _lobbyPlayerCount) { lobbyPlayerCount = _lobbyPlayerCount; }
	FORCEINLINE void SetPointPerPlayer(TMap<FString, int> _pointPerPlayer) { pointPerPlayer = _pointPerPlayer; }

	FORCEINLINE int GetLobbyPlayerCount() { return lobbyPlayerCount; }
	FORCEINLINE FLevelInfo GetCarrouselMap() { return carrouselMap; }
	FORCEINLINE TArray<FLevelInfo> GetAllBattleMap() { return allBattleMap; }
	FORCEINLINE TMap<FString, int> GetPointPerPlayer() { return pointPerPlayer; }

	FLevelInfo FindLevelInfoWithLevelName(FString _levelName);

	void ResetPointForAllPlayer();

	virtual void Init() override;
	
};
