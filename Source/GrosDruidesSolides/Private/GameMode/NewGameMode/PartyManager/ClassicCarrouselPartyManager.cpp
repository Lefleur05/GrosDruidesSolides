// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NewGameMode/PartyManager/ClassicCarrouselPartyManager.h"
#include "Utility.h"
#include "Player/PlayerCharacter.h"
#include <GameInstance/InfoPlayerGameInstance.h>
#include <Kismet/GameplayStatics.h>
#include "GameMode/PlayerState/InfoPlayerState.h"
#include "Player/Component/SpellComponent.h"
#include "GameMode/NewGameMode/AGDSGameMode.h"

// Sets default values
AClassicCarrouselPartyManager::AClassicCarrouselPartyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AClassicCarrouselPartyManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClassicCarrouselPartyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClassicCarrouselPartyManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CLEAR_TIMER(timerFreeLastPlayer);
	CLEAR_TIMER(timerFreeMidPlayer);
	CLEAR_TIMER(timerFreeFirstPlayer);
}

void AClassicCarrouselPartyManager::InitParty(const TArray<TObjectPtr<APlayerCharacter>>& _players)
{
	Super::InitParty(_players);
	const int _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			LOG_ERROR("InitParty-> AddDynamic onAddItem");
			_player->onAddItem.RemoveDynamic(this, &AClassicCarrouselPartyManager::ObjectTake);
			_player->onAddItem.AddDynamic(this, &AClassicCarrouselPartyManager::ObjectTake);

			_player->SetCanMove(false);
			_player->SetCanTakeDamage(false);
			_player->SetCanDash(false);
			if (_player->GetSpellComponent())
				_player->GetSpellComponent()->SetCanShoot(false);

			//LOG("Player Bind => " + FString::SanitizeFloat(_i));
			//pointPerPlayer.Add(allPlayers[_i], 0);


		}
	}
	if(gameMode)
		gameMode->SetPlayerLocationAndRotation();
}

void AClassicCarrouselPartyManager::StartParty()
{
	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			_player->SetCanMove(true);
			_player->SetCanTakeDamage(false);
			_player->SetCanDash(false);
			if (_player->GetSpellComponent())
				_player->GetSpellComponent()->SetCanShoot(false);
		}
	}
	gameMode->SetPlayerLocationAndRotation();
	TriPlayerPerPoint();
	//StartCountDown();
	countObjectTake = 0;

	TIMER(this, timerFreeLastPlayer, FreeLastPlayer(), timeForLastPlayer);
	TIMER(this, timerFreeMidPlayer, FreeMidPlayer(), timeForMidPlayer);
	TIMER(this, timerFreeFirstPlayer, FreeFirstPlayer(), timeForFirstPlayer);

}

void AClassicCarrouselPartyManager::EndParty()
{

}

void AClassicCarrouselPartyManager::TriPlayerPerPoint()
{
	int _lastPlayerPoint = 0;
	int _firstPlayerPoint = 1;

	firstPlayers.Empty();
	midPlayers.Empty();
	lastPlayers.Empty();

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

void AClassicCarrouselPartyManager::FreeLastPlayer()
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

void AClassicCarrouselPartyManager::FreeMidPlayer()
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

void AClassicCarrouselPartyManager::FreeFirstPlayer()
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

void AClassicCarrouselPartyManager::ObjectTake(APlayerCharacter* _player, FItem _item)
{
	if (!HasAuthority())return;
	countObjectTake++;

	LOG_ERROR("CountObjectTake " + FString::SanitizeFloat(countObjectTake) + " == " + FString::SanitizeFloat(allPlayers.Num()));
	if (countObjectTake >= allPlayers.Num())
	{
		if (timeBeforeSwitch >= 0.2f)
		{
			TIMER(this, timerSwitchLevel, SwitchToBattle(), timeBeforeSwitch);
			if(gameMode)
				gameMode->StartCountDown(timeBeforeSwitch);
		}
		else
		{
			SwitchToBattle();
		}
	}
}

void AClassicCarrouselPartyManager::SwitchToBattle()
{
	if (!HasAuthority())return;
	TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>();
	if (!_gameInstance)
	{
		WORLD->ServerTravel("LVL_TestLobby");
		return;
	}
	if (_gameInstance->GetAllBattleMap().Num() <= 0)
	{
		WORLD->ServerTravel(_gameInstance->GoToLobby());
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
	WORLD->ServerTravel(_targetLevel);
}
