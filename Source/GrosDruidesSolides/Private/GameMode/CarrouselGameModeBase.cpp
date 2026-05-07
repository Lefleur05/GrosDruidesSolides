// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/CarrouselGameModeBase.h"
#include "Utility.h"
#include "Player/PlayerCharacter.h"
#include <GameInstance/InfoPlayerGameInstance.h>
#include <Kismet/GameplayStatics.h>
#include "PlayerState/InfoPlayerState.h"
#include "Player/Component/SpellComponent.h"

ACarrouselGameModeBase::ACarrouselGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACarrouselGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void ACarrouselGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACarrouselGameModeBase::NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player)
{
	Super::NotifyPawnPossessed(_playerController, _player);
	bool _allplayerconnected = false;

	if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
	{
		_allplayerconnected = allPlayers.Num() == _gameInstance->GetLobbyPlayerCount();
		LOG("All Player Connected = " + FString::FromInt(allPlayers.Num()) + " == " + FString::SanitizeFloat(_gameInstance->GetLobbyPlayerCount()));
		carrouselSpawnPoint = _gameInstance->FindLevelInfoWithLevelName(UGameplayStatics::GetCurrentLevelName(GetWorld(), true)).allSpawnPoint;
		pointPerPlayer = _gameInstance->GetPointPerPlayer();
	}
	if (_allplayerconnected && WORLD)
	{
		Init();
		StartCarrousel();
	}
}

void ACarrouselGameModeBase::Init()
{
	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			_player->onAddItem.RemoveDynamic(this, &ACarrouselGameModeBase::ObjectTake);
			_player->onAddItem.AddDynamic(this, &ACarrouselGameModeBase::ObjectTake);

			_player->SetCanMove(false);
			_player->SetCanTakeDamage(false);
			_player->SetCanDash(false);
			if (_player->GetSpellComponent())
				_player->GetSpellComponent()->SetCanShoot(false);

			//LOG("Player Bind => " + FString::SanitizeFloat(_i));
			//pointPerPlayer.Add(allPlayers[_i], 0);

			if (allSpawnPoint.Num() >= 1.0f)
			{
				int _y = _i % allSpawnPoint.Num();
				_player->SetActorLocation(allSpawnPoint[_y]);

			}
		}
	}
}

void ACarrouselGameModeBase::TriPlayerPerPoint()
{
	int _lastPlayerPoint = 0;
	int _firstPlayerPoint = 1;

	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (IsValid(allPlayers[_i]))
		{
			FString _playerName = allPlayers[_i]->GetController()->PlayerState->GetUniqueId().ToString();

			if (pointPerPlayer.Contains(_playerName))
			{
				if (pointPerPlayer[_playerName] < _lastPlayerPoint)
				{
					_lastPlayerPoint = pointPerPlayer[_playerName];
				}
				else if (pointPerPlayer[_playerName] > _firstPlayerPoint)
				{
					_firstPlayerPoint = pointPerPlayer[_playerName];
				}
			}
		}
	}

	for (int _i = 0; _i < _count; _i++)
	{
		if (IsValid(allPlayers[_i]))
		{
			FString _playerName = allPlayers[_i]->GetController()->PlayerState->GetUniqueId().ToString();

			if (pointPerPlayer.Contains(_playerName) && pointPerPlayer[_playerName] <= _lastPlayerPoint)
			{
				lastPlayers.Add(allPlayers[_i]);
			}
			else if (pointPerPlayer.Contains(_playerName) && pointPerPlayer[_playerName] >= _firstPlayerPoint)
			{
				firstPlayers.Add(allPlayers[_i]);
			}
			else
			{
				midPlayers.Add(allPlayers[_i]);
			}
		}
	}
}

void ACarrouselGameModeBase::FreeLastPlayer()
{
	const int& _count = lastPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = lastPlayers[_i];

		if (_player)
		{
			_player->SetCanMove(true);
			_player->SetCanTakeDamage(false);
			_player->SetCanDash(true);
			if (_player->GetSpellComponent())
				_player->GetSpellComponent()->SetCanShoot(true);
		}
	}
}

void ACarrouselGameModeBase::FreeMidPlayer()
{
	const int& _count = midPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = midPlayers[_i];

		if (_player)
		{
			_player->SetCanMove(true);
			_player->SetCanTakeDamage(false);
			_player->SetCanDash(true);
			if (_player->GetSpellComponent())
				_player->GetSpellComponent()->SetCanShoot(true);
		}
	}
}

void ACarrouselGameModeBase::FreeFirstPlayer()
{
	const int& _count = firstPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = firstPlayers[_i];

		if (_player)
		{
			_player->SetCanMove(true);
			_player->SetCanTakeDamage(false);
			_player->SetCanDash(true);
			if (_player->GetSpellComponent())
				_player->GetSpellComponent()->SetCanShoot(true);
		}
	}
}

void ACarrouselGameModeBase::StartCarrousel()
{
	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			_player->SetCanMove(false);
			_player->SetCanTakeDamage(false);
			_player->SetCanDash(false);
			if (_player->GetSpellComponent())
				_player->GetSpellComponent()->SetCanShoot(false);

			if (carrouselSpawnPoint.Num() > 0)
				allPlayers[_i]->SetActorLocation(carrouselSpawnPoint[_i % carrouselSpawnPoint.Num()]);
		}
	}
	TriPlayerPerPoint();
	countObjectTake = 0;

	TIMER(this, timerFreeLastPlayer, FreeLastPlayer(), timeForLastPlayer);
	TIMER(this, timerFreeMidPlayer, FreeMidPlayer(), timeForMidPlayer);
	TIMER(this, timerFreeFirstPlayer, FreeFirstPlayer(), timeForFirstPlayer);

}

void ACarrouselGameModeBase::ObjectTake(APlayerCharacter* _player)
{
	countObjectTake++;

	//LOG("CountObjectTake " + FString::SanitizeFloat(countObjectTake) + " == " + FString::SanitizeFloat(allPlayers.Num()));
	if (countObjectTake >= allPlayers.Num())
	{
		SwitchToBattle();
	}
}

void ACarrouselGameModeBase::SwitchToBattle()
{
	if (!HasAuthority())return;
	TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>();
	if (!_gameInstance)return;
	if (_gameInstance->GetAllBattleMap().Num() <= 0)
	{
		WORLD->ServerTravel("LVL_Lobby?game=/Game/Blueprint/GameMode/BP_LobbyGameModeBase.BP_LobbyGameModeBase_C");
		return;
	}

	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (IsValid(allPlayers[_i]))
		{
			if (TObjectPtr<AInfoPlayerState> _playerState = Cast<AInfoPlayerState>(allPlayers[_i]->GetController()->PlayerState))
			{
				_playerState->SetCurrentItems(allPlayers[_i]->GetCurrentItems());
				if (_gameInstance->GetPointPerPlayer().Contains(_playerState->GetUniqueId().ToString()))
					_playerState->SetPoint(_gameInstance->GetPointPerPlayer()[_playerState->GetUniqueId().ToString()]);
			}
		}
	}

	int _mapIndex = UKismetMathLibrary::RandomIntegerInRange(0.0f, _gameInstance->GetAllBattleMap().Num()-1);
	FString _targetLevel = _gameInstance->GetAllBattleMap()[_mapIndex].map;

	LOG("Server Travel to => " + _targetLevel);
	WORLD->ServerTravel(_targetLevel + "?game=/Game/Blueprint/GameMode/BP_BattleGameModeBase.BP_BattleGameModeBase_C");
}
