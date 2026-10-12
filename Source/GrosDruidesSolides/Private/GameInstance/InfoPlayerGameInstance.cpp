// Fill out your copyright notice in the Description page of Project Settings.
#include "GameInstance/InfoPlayerGameInstance.h"
#include "GameAnalytics.h"
#include "GameAnalyticsModule.h"
#include "Sound/SoundBase.h"
#include "Utility.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void UInfoPlayerGameInstance::Init()
{
    Super::Init();
    TIMER(this, startMusique, PlayRandomBattleMusique(), 1.0f);

    UGameAnalytics* _gameAnalytics = FGameAnalyticsModule::Get().GetInstance();

    // initialize the SDK with your keys
    _gameAnalytics->SetEnabledInfoLog(true);


    _gameAnalytics->Initialize(GA_GAME_KEY, GA_SECRET_KEY);
}

void UInfoPlayerGameInstance::OnStart()
{
    Super::OnStart();

}

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

void UInfoPlayerGameInstance::SetCurrentAudioVolume(float _volume)
{
    musicVolume = _volume;

    if (_volume <= 0.0f)
    {
        currentAudio->SetPaused(true);
        musicIsPaused = true;
        PAUSE_TIMER(startMusique);
    }
    else if(musicIsPaused)
    {
        currentAudio->SetPaused(false);
        musicIsPaused = false;
        UNPAUSE_TIMER(startMusique);
    }

    currentAudio->SetVolumeMultiplier(_volume);
}

void UInfoPlayerGameInstance::ResetPointForAllPlayer()
{
    pointPerPlayer = TMap<FString, int>();
    /*for (TMap<FString, int32>::TIterator _player = pointPerPlayer.CreateIterator(); _player; ++_player)
    {
        _player.Value() = 0;
    }*/
}

void UInfoPlayerGameInstance::PlayRandomBattleMusique()
{
    LOG("PlayRandomBattleMusique");
    if (allBattleMusique.Num() <= 0)return;
    const int& _randomMusicIndex = UKismetMathLibrary::RandomIntegerInRange(0, allBattleMusique.Num()-1);
    TObjectPtr<USoundBase> _musicToPlay = allBattleMusique[_randomMusicIndex];

    if (!_musicToPlay || !WORLD)return;

    if (currentAudio && IsValid(this))
    {
        currentAudio->OnAudioFinished.RemoveDynamic(this, &UInfoPlayerGameInstance::PlayRandomBattleMusique);
        currentAudio->DestroyComponent();
        currentAudio = nullptr;
    }

    currentAudio = UGameplayStatics::SpawnSound2D(WORLD, _musicToPlay,0.2f,1.0f,0.0f,nullptr,true, false);
    currentAudio->SetVolumeMultiplier(musicVolume);
    //currentAudio = UGameplayStatics::SpawnSound2D(WORLD, _musicToPlay);

    if (!currentAudio || !IsValid(this))return;
    LOG("Music");
    const float& _duration = _musicToPlay->GetDuration();
    TIMER(this, startMusique, PlayRandomBattleMusique(), _duration);
    //currentAudio->OnAudioFinished.AddDynamic(this, &UInfoPlayerGameInstance::PlayRandomBattleMusique);

}

FString UInfoPlayerGameInstance::GetRandomLevel()
{
    TArray<FLevelInfo> battleMapAvailable = allBattleMap;
    battleMapAvailable.Remove(previousMap);

    int _mapIndex = UKismetMathLibrary::RandomIntegerInRange(0.0f, battleMapAvailable.Num() - 1);
    previousMap = battleMapAvailable[_mapIndex];

    return previousMap.map;
}

FString UInfoPlayerGameInstance::GoToLobby()
{
    previousMap = FLevelInfo();
    return "LVL_TestLobby";
}
