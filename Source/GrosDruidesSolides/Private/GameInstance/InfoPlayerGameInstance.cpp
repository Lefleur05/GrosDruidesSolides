// Fill out your copyright notice in the Description page of Project Settings.
#include "GameInstance/InfoPlayerGameInstance.h"
#include "GameAnalytics.h"
#include "GameAnalyticsModule.h"



FLevelInfo UInfoPlayerGameInstance::FindLevelInfoWithLevelName(FString _levelName)
{
    if (carrouselMap.map == _levelName)
        return carrouselMap;
    const int& _count = allBattleMap.Num();
    for (int _i = 0; _i < _count; _i++)
    {
        if (allBattleMap[_i].map == _levelName)
            return allBattleMap[_i];
    }

    return FLevelInfo();
}

void UInfoPlayerGameInstance::ResetPointForAllPlayer()
{
    pointPerPlayer = TMap<FString, int>();
    /*for (TMap<FString, int32>::TIterator _player = pointPerPlayer.CreateIterator(); _player; ++_player)
    {
        _player.Value() = 0;
    }*/
}

void UInfoPlayerGameInstance::Init()
{
    Super::Init();
    UGameAnalytics* _gameAnalytics = FGameAnalyticsModule::Get().GetInstance();

    // initialize the SDK with your keys
    _gameAnalytics->SetEnabledInfoLog(true);



    _gameAnalytics->Initialize(GA_GAME_KEY, GA_SECRET_KEY);
}
