// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/CarrouselGameModeBase.h"
#include "Utility.h"
#include "Player/PlayerCharacter.h"
#include <GameInstance/InfoPlayerGameInstance.h>
#include <Kismet/GameplayStatics.h>
#include "PlayerState/InfoPlayerState.h"
#include "Player/Component/SpellComponent.h"
#include "CarrouselGameModeBase.h"

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

void ACarrouselGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CLEAR_TIMER(timerFreeLastPlayer);
	CLEAR_TIMER(timerFreeMidPlayer);
	CLEAR_TIMER(timerFreeFirstPlayer);
}

void ACarrouselGameModeBase::NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player)
{
	Super::NotifyPawnPossessed(_playerController, _player);
	bool _allplayerconnected = false;

	if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
	{
		_allplayerconnected = allPlayers.Num() == _gameInstance->GetLobbyPlayerCount();
		//LOG("All Player Connected = " + FString::FromInt(allPlayers.Num()) + " == " + FString::SanitizeFloat(_gameInstance->GetLobbyPlayerCount()));
		allSpawnPoint = _gameInstance->FindLevelInfoWithLevelName(UGameplayStatics::GetCurrentLevelName(GetWorld(), true)).allSpawnPoint;
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
	const int _count = allPlayers.Num();
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

			
		}
	}
	SetPlayerLocationAndRotation();
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
		}
	}
	SetPlayerLocationAndRotation();
	TriPlayerPerPoint();
	//StartCountDown();
	countObjectTake = 0;

	TIMER(this, timerFreeLastPlayer, FreeLastPlayer(), timeForLastPlayer);
	TIMER(this, timerFreeMidPlayer, FreeMidPlayer(), timeForMidPlayer);
	TIMER(this, timerFreeFirstPlayer, FreeFirstPlayer(), timeForFirstPlayer);

}

/*void ACarrouselGameModeBase::StartCountDown()
{
	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{

			if (firstPlayers.Contains(_player))
			{
				_player->Client_StartCountDownUI(timeForFirstPlayer);
			}
			else if (midPlayers.Contains(_player))
			{
				_player->Client_StartCountDownUI(timeForMidPlayer);
			}
			else if (lastPlayers.Contains(_player))
			{
				_player->Client_StartCountDownUI(timeForLastPlayer);
			}
		}
	}

}*/

void ACarrouselGameModeBase::TriPlayerPerPoint()
{
	int _lastPlayerPoint = 0;
	int _firstPlayerPoint = 1;

	const int _count = allPlayers.Num();
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
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			FString _playerName = _player->GetController()->PlayerState->GetUniqueId().ToString();

			if (pointPerPlayer.Contains(_playerName) && pointPerPlayer[_playerName] <= _lastPlayerPoint)
			{
				lastPlayers.Add(_player);
				_player->Client_StartCountDownUI(timeForLastPlayer);
			}
			else if (pointPerPlayer.Contains(_playerName) && pointPerPlayer[_playerName] >= _firstPlayerPoint)
			{
				firstPlayers.Add(_player);
				_player->Client_StartCountDownUI(timeForFirstPlayer);
			}
			else
			{
				midPlayers.Add(_player);
				_player->Client_StartCountDownUI(timeForMidPlayer);
			}
		}
	}
}

void ACarrouselGameModeBase::FreeLastPlayer()
{
	const int _count = lastPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (!lastPlayers.IsValidIndex(_i)) continue;
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
	const int _count = midPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (!midPlayers.IsValidIndex(_i)) continue;
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
	const int _count = firstPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (!firstPlayers.IsValidIndex(_i)) continue;
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

void ACarrouselGameModeBase::ObjectTake(APlayerCharacter* _player, FItem _item)
{
	if (!HasAuthority())return;
	countObjectTake++;

	//LOG("CountObjectTake " + FString::SanitizeFloat(countObjectTake) + " == " + FString::SanitizeFloat(allPlayers.Num()));
	if (countObjectTake >= allPlayers.Num())
	{
		if (timeBeforeSwitch >= 0.2f)
		{
			TIMER(this, timerSwitchLevel, SwitchToBattle(), timeBeforeSwitch);
			StartCountDown(timeBeforeSwitch);
		}
		else
		{
			SwitchToBattle();
		}
	}
}

void ACarrouselGameModeBase::SwitchToBattle()
{
	if (!HasAuthority())return;
	TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>();
	if (!_gameInstance)
	{
		WORLD->ServerTravel("LVL_TestLobby?game=/Game/Blueprint/GameMode/BP_LobbyGameModeBase.BP_LobbyGameModeBase_C");
		return;
	}
	if (_gameInstance->GetAllBattleMap().Num() <= 0)
	{
		WORLD->ServerTravel(_gameInstance->GoToLobby() + "?game=/Game/Blueprint/GameMode/BP_CarrouselGameModeBase.BP_CarrouselGameModeBase_C");
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

	FString _targetLevel = _gameInstance->GetRandomLevel();

	LOG_CUSTOM("Server Travel to => " + _targetLevel, Emerald);
	WORLD->ServerTravel(_targetLevel + "?game=/Game/Blueprint/GameMode/BP_BattleGameModeBase.BP_BattleGameModeBase_C");
}
