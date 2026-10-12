// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/PlayerState/InfoPlayerState.h"
#include <Net/UnrealNetwork.h>
#include "GameMode/GameState/InfoGameState.h"
#include "Player/PlayerCharacter.h"
#include "Utility.h"

void AInfoPlayerState::BeginPlay()
{
	Super::BeginPlay();
	onUpdatePoint.Broadcast(point);
}

void AInfoPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInfoPlayerState, playerColor)
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
		_InfoPlayerState->playerColor = playerColor;
	}
}

void AInfoPlayerState::OnRep_Point()
{
	onUpdatePoint.Broadcast(point);
}

void AInfoPlayerState::OnRep_PlayerColor()
{
	UpdatePlayerColor();
}

void AInfoPlayerState::UpdatePlayerColor()
{
	LOG("UpdatePlayerColor");
	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(GetPawn());
	if (!_player)
	{
		LOG_ERROR("No Player"); // call at initialisation
		return;
	}
	TObjectPtr<USkeletalMeshComponent> _mesh = _player->GetMesh();
	if (!_mesh)
	{
		LOG_ERROR("No Player Mesh");
		return;
	}

	FLinearColor _color = colorForPlayer[playerColor];

	LOG(_color.ToString());

	TObjectPtr<UMaterialInstanceDynamic> _dynamicMaterial = _mesh->CreateDynamicMaterialInstance(0);
	_dynamicMaterial->SetVectorParameterValue(FName("Paint Tint"), _color);
	_mesh->SetMaterial(0, _dynamicMaterial);

	_dynamicMaterial = _mesh->CreateDynamicMaterialInstance(1);
	if(_dynamicMaterial)
	{
		_dynamicMaterial->SetVectorParameterValue(FName("Paint Tint"), _color);
		_mesh->SetMaterial(1, _dynamicMaterial);
	}
	
}

void AInfoPlayerState::ResetForLobby()
{
	point = 0;
	currentItems = TArray<FItem>();
}
