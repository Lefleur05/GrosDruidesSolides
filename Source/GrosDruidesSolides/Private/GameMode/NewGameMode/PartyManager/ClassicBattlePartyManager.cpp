// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NewGameMode/PartyManager/ClassicBattlePartyManager.h"
#include "Player/PlayerCharacter.h"
#include "Utility.h"
#include <Kismet/GameplayStatics.h>
#include <GPE/Carrousel.h>
#include <GameInstance/InfoPlayerGameInstance.h>
#include "GameFramework/PlayerState.h"
#include "GameMode/PlayerState/InfoPlayerState.h"
#include "Player/Component/SpellComponent.h"
#include "GameMode/NewGameMode/AGDSGameMode.h"


// Sets default values
AClassicBattlePartyManager::AClassicBattlePartyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AClassicBattlePartyManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClassicBattlePartyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClassicBattlePartyManager::InitParty(const TArray<TObjectPtr<APlayerCharacter>>& _players)
{
	Super::InitParty(_players);
	const int _count = allPlayers.Num();
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

			_player->onDeath.RemoveDynamic(this, &AClassicBattlePartyManager::DeathPlayer);
			_player->onDeath.AddDynamic(this, &AClassicBattlePartyManager::DeathPlayer);
			//pointPerPlayer.Add(allPlayers[_i], 0);
		}
	}
	if(gameMode)
		gameMode->SetPlayerLocationAndRotation();
}

void AClassicBattlePartyManager::StartParty()
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
			_player->Client_StartCountDownUI(timeForStartBattle);
		}
	}
	if(gameMode)
		gameMode->SetPlayerLocationAndRotation();
	timeStartCurrentLevel = WORLD->GetTimeSeconds();

	TIMER(this, timerForStartBattle, FreeForBattle(), timeForStartBattle);
}

void AClassicBattlePartyManager::EndParty()
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

	if (timeBeforeSwitch >= 0.2f)
	{
		TIMER(this, timerSwitchLevel, SwitchToCarrousel(), timeBeforeSwitch);
		if(gameMode)
			gameMode->StartCountDown(timeBeforeSwitch);
	}
	else
	{
		SwitchToCarrousel();
	}
}

void AClassicBattlePartyManager::FreeForBattle()
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

int AClassicBattlePartyManager::GetCountPlayerAlive(TObjectPtr<APlayerCharacter>& _lastPlayerStanding)
{
	int _countPlayerAlive = 0;
	const int _count = allPlayers.Num();

	for (int _i = 0; _i < _count; _i++)
	{
		if (IsValid(allPlayers[_i]) && !allPlayers[_i]->GetIsDead())
		{
			_countPlayerAlive++;
			_lastPlayerStanding = allPlayers[_i];
		}
	}
	return _countPlayerAlive;
}

bool AClassicBattlePartyManager::AddPlayerPoint(TObjectPtr<APlayerCharacter> _lastPlayerStanding)
{
	TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>();
	TObjectPtr<APlayerState> _playerState = _lastPlayerStanding ? _lastPlayerStanding->GetPlayerState() : nullptr;

	if (!_gameInstance || !_lastPlayerStanding || !_playerState)
		return false;
	FString _playerName = _playerState->GetUniqueId().ToString();
	if (_playerName.IsEmpty())
		return false;

	TMap<FString, int> _point = _gameInstance->GetPointPerPlayer();
	//LOG_CUSTOM(_playerName + (_point.Contains(_playerName) ? " is containe " : " is not containe "), Blue);

	if (!_point.Contains(_playerName))
	{
		LOG_ERROR("Round Winner by Player " + _playerName + " but not found in TMap ");
		return false;
	}

	_point[_playerName] += 1;
	_gameInstance->SetPointPerPlayer(_point);

	if (TObjectPtr<AInfoPlayerState> _infoPlayerState = Cast<AInfoPlayerState>(_playerState))
		_infoPlayerState->SetPoint(_point[_playerName]);

	LOG_CUSTOM("Round Winner Player Name => " + _playerName + " Count Point => " + FString::FromInt(_point[_playerName]), Emerald);

	if (_point[_playerName] >= pointForWin)
	{
		LOG_CUSTOM(_lastPlayerStanding.GetName() + " Win the game", Emerald);
		return true;
	}
	return false;
}

void AClassicBattlePartyManager::DeathPlayer(APlayerCharacter* _player)
{
	if (roundEnded)return;

	TObjectPtr<APlayerCharacter> _lastPlayerStanding = nullptr;
	int _countPlayerAlive = GetCountPlayerAlive(_lastPlayerStanding);

	//LOG("Count Player alive => "+ FString::FromInt(_countPlayerAlive));

	if (_countPlayerAlive == 0) //the last ones killed each other
	{
		roundEnded = true;
		CLEAR_TIMER(timerBackToLobby);
		TIMER(this, timerEndBattle, EndParty(), 1.0f);
		return;
	}
	if (_countPlayerAlive != 1)return; //There are other players still in the game

	roundEnded = true;

	bool _gameWin = AddPlayerPoint(_lastPlayerStanding);

	if (_gameWin)
	{
		TIMER(this, timerBackToLobby, BackToLobby(), 1.0f);
	}
	else
	{
		TIMER(this, timerEndBattle, EndParty(), 1.0f);
	}
}

void AClassicBattlePartyManager::BackToLobby()
{
	TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>();
	if (!_gameInstance)
	{
		LOG_ERROR("SwitchToCarrousel: GameInstance invalide");
		return;
	}

	LOG_CUSTOM("Server Travel to => LVL_TestLobby", Emerald);
	WORLD->ServerTravel(_gameInstance->GoToLobby());
	//WORLD->ServerTravel("LVL_TestLobby?game=/Game/Blueprint/GameMode/BP_LobbyGameModeBase.BP_LobbyGameModeBase_C");
}

void AClassicBattlePartyManager::SwitchToCarrousel()
{
	TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>();
	if (!_gameInstance)
	{
		LOG_ERROR("SwitchToCarrousel: GameInstance invalide");
		return;
	}
	// If carrousel is not valid the game not started
	FLevelInfo _carrousel = _gameInstance->GetCarrouselMap();
	if (_carrousel.map.IsEmpty())
	{
		LOG_ERROR("GetCarrouselMap is Empty: GameInstance invalide");
		BackToLobby();
		return;
	}

	LOG_CUSTOM("Server Travel to => " + _gameInstance->GetCarrouselMap().map, Emerald);
	WORLD->ServerTravel(_gameInstance->GetCarrouselMap().map);
}

