// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/PlayerState/InfoPlayerState.h"
#include <Net/UnrealNetwork.h>

void AInfoPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInfoPlayerState, point)
	DOREPLIFETIME(AInfoPlayerState, currentItems)
}

void AInfoPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	TObjectPtr<AInfoPlayerState> _InfoPlayerState = Cast<AInfoPlayerState>(PlayerState);
	if (_InfoPlayerState)
	{
		_InfoPlayerState->point = point;
		_InfoPlayerState->currentItems = currentItems;
	}
}

void AInfoPlayerState::ResetForLobby()
{
	point = 0;
	currentItems = TArray<FItem>();
}
