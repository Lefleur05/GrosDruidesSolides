// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BattleGameModeBase.h"
#include "Player/PlayerCharacter.h"
#include "Utility.h"
#include <Kismet/GameplayStatics.h>
#include <GPE/Carrousel.h>
#include <GameInstance/InfoPlayerGameInstance.h>
#include "GameFramework/PlayerState.h"
#include "PlayerState/InfoPlayerState.h"
#include "Player/Component/SpellComponent.h"

// Sets default values
ABattleGameModeBase::ABattleGameModeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABattleGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
//void ABattleGameModeBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime); 
//	//Desactivate with PrimaryActorTick.bCanEverTick = false
//}

void ABattleGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CLEAR_TIMER(timerForStartBattle);
	CLEAR_TIMER(timerEndBattle);
	CLEAR_TIMER(startBattleTimer);
}

void ABattleGameModeBase::NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player)
{
	Super::NotifyPawnPossessed(_playerController, _player);
	bool _allplayerconnected = false;

	if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
	{
		_allplayerconnected = allPlayers.Num() == _gameInstance->GetLobbyPlayerCount();
		LOG("All Player Connected = " + FString::FromInt(allPlayers.Num()) + " == " + FString::FromInt(_gameInstance->GetLobbyPlayerCount()));
	}
	if (_allplayerconnected && WORLD && HasAuthority())
	{
		if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
			spawnPoint = _gameInstance->FindLevelInfoWithLevelName(UGameplayStatics::GetCurrentLevelName(GetWorld(), true)).allSpawnPoint;

		Init();
		StartBattle();
	}
}

void ABattleGameModeBase::Init()
{
	const int _count = allPlayers.Num();
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

			_player->onDeath.RemoveDynamic(this, &ABattleGameModeBase::DeathPlayer);
			_player->onDeath.AddDynamic(this, &ABattleGameModeBase::DeathPlayer);
			//pointPerPlayer.Add(allPlayers[_i], 0);

			//if (allSpawnPoint.Num() >= 1)
			//{
			//	int _y = _i % allSpawnPoint.Num();
			//	allPlayers[_i]->SetActorLocation(allSpawnPoint[_y]);

			//}
		}
	}
}

void ABattleGameModeBase::StartBattle()
{
	roundEnded = false;
	const int _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			_player->SetCanMove(false);
			_player->SetCanTakeDamage(true);
			_player->SetCanDash(false);
			if (_player->GetSpellComponent())
				_player->GetSpellComponent()->SetCanShoot(false);

			if(spawnPoint.Num() > 0)
				allPlayers[_i]->SetActorLocation(spawnPoint[_i % spawnPoint.Num()]);
		}
	}
	timeStartCurrentLevel = WORLD->GetTimeSeconds();

	TIMER(this, timerForStartBattle, FreeForBattle(), timeForStartBattle);
}

void ABattleGameModeBase::FreeForBattle()
{
	const int _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			_player->SetCanMove(true);
			_player->SetCanTakeDamage(true);
			_player->SetCanDash(true);
			if (_player->GetSpellComponent())
				_player->GetSpellComponent()->SetCanShoot(true);
		}
	}
}

void ABattleGameModeBase::DeathPlayer(APlayerCharacter* _player)
{
	if (roundEnded)return;
	int _countPlayerAlive = 0;
	const int _count = allPlayers.Num();
	TObjectPtr<APlayerCharacter> _lastPlayerStanding = nullptr;
	for (int _i = 0; _i < _count; _i++)
	{
		if (IsValid(allPlayers[_i]) && !allPlayers[_i]->GetIsDead())
		{
			_countPlayerAlive++;
			_lastPlayerStanding = allPlayers[_i];
		}
	}
	//LOG("Count Player alive => "+ FString::FromInt(_countPlayerAlive));

	if (_countPlayerAlive == 0)
	{
		roundEnded = true;
		TIMER(this, timerEndBattle, EndBattle(), 1.0f);
		return;
	}
	if (_countPlayerAlive != 1)return;

	roundEnded = true;

	if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
	{
		TMap<FString, int> _point = _gameInstance->GetPointPerPlayer();
		if (!_lastPlayerStanding || !_lastPlayerStanding->GetPlayerState())
		{
			TIMER(this, timerEndBattle, EndBattle(), 1.0f);
			return;
		}
		FString _playerName = _lastPlayerStanding->GetPlayerState()->GetUniqueId().ToString();
		if(_playerName.IsEmpty())
		{
			TIMER(this, timerEndBattle, EndBattle(), 1.0f);
			return;
		}

		bool _isContaine = _point.Contains(_playerName);

		//LOG_CUSTOM(_playerName + (_isContaine ? " is containe " : " is not containe "), Blue);

		if (_isContaine)
		{
			_point[_playerName] += 1;
			_gameInstance->SetPointPerPlayer(_point);

			LOG("Round Winner Player Name => " + _playerName  + " Cout Point => " + FString::FromInt(_point[_playerName]));

			if (_point[_playerName] >= pointForWin)
			{
				LOG_CUSTOM(_lastPlayerStanding.GetName() + " Win the game", Emerald);
				TIMER(this, timerEndBattle, BackToLobby(), 1.0f);
				return;
			}
		}
		else
		{
			LOG_ERROR("Round Winner by Player " + _playerName + " but not found in TMap ");
		}
	}
	TIMER(this, timerEndBattle, EndBattle(), 1.0f);
}

void ABattleGameModeBase::EndBattle()
{
	const int _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (IsValid(allPlayers[_i]) && allPlayers[_i]->GetPlayerState())
		{
			if (TObjectPtr<AInfoPlayerState> _playerState = Cast<AInfoPlayerState>(allPlayers[_i]->GetPlayerState()))
			{
				_playerState->SetCurrentItems(allPlayers[_i]->GetCurrentItems());
				if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
				{
					TMap<FString, int> _point = _gameInstance->GetPointPerPlayer();
					FString _playerName = _playerState->GetUniqueId().ToString();
					if (_point.Contains(_playerName))
						_playerState->SetPoint(_point[_playerName]);
				}
			}
		}
	}

	SwitchToCarrousel();
}

void ABattleGameModeBase::BackToLobby()
{
	LOG("Server Travel to => LVL_Lobby");
	WORLD->ServerTravel("LVL_Lobby?game=/Game/Blueprint/GameMode/BP_LobbyGameModeBase.BP_LobbyGameModeBase_C");
}

void ABattleGameModeBase::SwitchToCarrousel()
{
	TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>();
	if (!_gameInstance)return;
	// If carrousel is not valid the game not started
	if (_gameInstance->GetCarrouselMap().map.IsEmpty())
	{
		BackToLobby();
		return;
	}

	LOG_CUSTOM("Server Travel to => " + _gameInstance->GetCarrouselMap().map, Black);
	WORLD->ServerTravel(_gameInstance->GetCarrouselMap().map + "?game=/Game/Blueprint/GameMode/BP_CarrouselGameModeBase.BP_CarrouselGameModeBase_C");
}
