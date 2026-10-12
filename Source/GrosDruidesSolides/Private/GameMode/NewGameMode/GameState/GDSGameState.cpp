// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NewGameMode/GameState/GDSGameState.h"

void AGDSGameState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);
    onPlayerArrayUpdateGDS.Broadcast();
}

void AGDSGameState::RemovePlayerState(APlayerState* PlayerState)
{
    Super::RemovePlayerState(PlayerState);
    onPlayerArrayUpdateGDS.Broadcast();
}
