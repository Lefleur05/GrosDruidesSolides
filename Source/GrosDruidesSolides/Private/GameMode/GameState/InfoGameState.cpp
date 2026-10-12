// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/GameState/InfoGameState.h"

void AInfoGameState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);
    onPlayerArrayUpdate.Broadcast();
}

void AInfoGameState::RemovePlayerState(APlayerState* PlayerState)
{
    Super::RemovePlayerState(PlayerState);
    onPlayerArrayUpdate.Broadcast();
}
