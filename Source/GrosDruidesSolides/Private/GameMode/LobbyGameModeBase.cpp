// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameModeBase.h"
#include "Player/PlayerCharacter.h"
#include "Utility.h"
#include "GameInstance/InfoPlayerGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "PlayerState/InfoPlayerState.h"
#include "Player/Component/SpellComponent.h"

void ALobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	//LOG("allSpawnPoint => " + FString::SanitizeFloat(allSpawnPoint.Num()));
	/*const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (IsValid(allPlayers[_i]))
		{
			allPlayers[_i]->SetCanMove(true);
			allPlayers[_i]->onDeath.RemoveDynamic(this, &ALobbyGameModeBase::RespawnPlayer);
			allPlayers[_i]->onDeath.AddDynamic(this, &ALobbyGameModeBase::RespawnPlayer);
			LOG_CUSTOM("player Binding in lobby", Yellow);

			if (allPlayers[_i]->GetController() && allPlayers[_i]->GetController()->PlayerState)
			{
				if (TObjectPtr<AInfoPlayerState> _playerState = Cast<AInfoPlayerState>(allPlayers[_i]->GetController()->PlayerState))
				{
					_playerState->SetCurrentItems(TArray<FItem>());
					LOG("ClearItem");
				}
			}
		}
	}

	if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
	{
		//LOG(FString::SanitizeFloat(_gameInstance->GetLobbyPlayerCount()) + " Player Count in lobby");
		if (!pointIsReset)
		{
			_gameInstance->ResetPointForAllPlayer(); // Reset seulement quand le 1er joueur arrive
			_gameInstance->SetLobbyPlayerCount(0);
			pointIsReset = true;
		}
		_gameInstance->SetLobbyPlayerCount(_gameInstance->GetLobbyPlayerCount() + 1);

		TMap<FString, int> _point = _gameInstance->GetPointPerPlayer();
		if (!NewPlayer || !NewPlayer->PlayerState)
		{
			LOG_ERROR("No Player or No Player state");
			return;
		}
		const FString _playerId = NewPlayer->PlayerState->GetUniqueId().ToString();

		if (!_point.Find(_playerId))
		{
			LOG_CUSTOM("Player connected " + _playerId, Blue);

			_point.Add(_playerId, 0);
			_gameInstance->SetPointPerPlayer(_point);
		}

		if (TObjectPtr<AInfoPlayerState> _playerState = Cast<AInfoPlayerState>(NewPlayer->PlayerState))
		{
			_playerState->ResetForLobby();
		}
	}*/
}

void ALobbyGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
	{
		_gameInstance->SetLobbyPlayerCount(_gameInstance->GetLobbyPlayerCount() - 1);
		//LOG(FString::SanitizeFloat(_gameInstance->GetLobbyPlayerCount()) + " Player Count in lobby");
		TMap<FString, int> _point = _gameInstance->GetPointPerPlayer();
		if (Exiting->PlayerState && _point.Find(Exiting->PlayerState->GetUniqueId().ToString()))
		{
			_point.Remove(Exiting->PlayerState->GetUniqueId().ToString());
			_gameInstance->SetPointPerPlayer(_point);
		}
	}
}

void ALobbyGameModeBase::NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player)
{
	Super::NotifyPawnPossessed(_playerController, _player);
	if (!_player || !_playerController)return;

	if (IsValid(_player))
	{
		_player->onDeath.RemoveDynamic(this, &ALobbyGameModeBase::RespawnPlayer);
		_player->onDeath.AddDynamic(this, &ALobbyGameModeBase::RespawnPlayer);

		_player->SetCanMove(true);
		_player->SetCanTakeDamage(true);
		_player->SetCanDash(true);
		if(_player->GetSpellComponent())
			_player->GetSpellComponent()->SetCanShoot(true);

		LOG_CUSTOM("player Binding in lobby", Yellow);

		if (_player->GetController() && _player->GetController()->PlayerState)
		{
			if (TObjectPtr<AInfoPlayerState> _playerState = Cast<AInfoPlayerState>(_player->GetController()->PlayerState))
			{
				_playerState->SetCurrentItems(TArray<FItem>());
				LOG("ClearItem");
			}
		}
	}

	if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
	{
		//LOG(FString::SanitizeFloat(_gameInstance->GetLobbyPlayerCount()) + " Player Count in lobby");
		if (!pointIsReset)
		{
			_gameInstance->ResetPointForAllPlayer(); // Reset seulement quand le 1er joueur arrive
			_gameInstance->SetLobbyPlayerCount(0);
			pointIsReset = true;
		}
		_gameInstance->SetLobbyPlayerCount(_gameInstance->GetLobbyPlayerCount() + 1);

		TMap<FString, int> _point = _gameInstance->GetPointPerPlayer();
		if (!_playerController || !_playerController->PlayerState)
		{
			LOG_ERROR("No Player or No Player state");
			return;
		}
		const FString _playerId = _playerController->PlayerState->GetUniqueId().ToString();

		if (!_point.Find(_playerId))
		{
			LOG_CUSTOM("Player connected " + _playerId, Blue);

			_point.Add(_playerId, 0);
			_gameInstance->SetPointPerPlayer(_point);
		}

		if (TObjectPtr<AInfoPlayerState> _playerState = Cast<AInfoPlayerState>(_playerController->PlayerState))
		{
			_playerState->ResetForLobby();
		}
	}
}

void ALobbyGameModeBase::RespawnPlayer(APlayerCharacter* _player)
{
	//if (!HasAuthority())return;

	if (_player->GetPlayerState())
		LOG_CUSTOM(_player->GetPlayerState()->GetUniqueId().ToString() + " Start timer for Respawn", Yellow);

	if(!GetWorldTimerManager().IsTimerActive(timerRespawn1))
		GetWorldTimerManager().SetTimer(timerRespawn1, [this, _player]() {DoRespawnPlayer(_player); }, respawnTime, false);
	else if (!GetWorldTimerManager().IsTimerActive(timerRespawn2))
		GetWorldTimerManager().SetTimer(timerRespawn2, [this, _player]() {DoRespawnPlayer(_player); }, respawnTime, false);
	else if (!GetWorldTimerManager().IsTimerActive(timerRespawn3))
		GetWorldTimerManager().SetTimer(timerRespawn3, [this, _player]() {DoRespawnPlayer(_player); }, respawnTime, false);
	else if (!GetWorldTimerManager().IsTimerActive(timerRespawn4))
		GetWorldTimerManager().SetTimer(timerRespawn4, [this, _player]() {DoRespawnPlayer(_player); }, respawnTime, false);
}

void ALobbyGameModeBase::DoRespawnPlayer(APlayerCharacter* _player)
{
	if (_player->GetPlayerState())
		LOG_CUSTOM(_player->GetPlayerState()->GetUniqueId().ToString() + " Respawn", Yellow);
	_player->Respawn();
}

void ALobbyGameModeBase::ClearItem()
{
	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (IsValid(allPlayers[_i]) && allPlayers[_i]->GetController() && allPlayers[_i]->GetController()->PlayerState)
		{
			if (TObjectPtr<AInfoPlayerState> _playerState = Cast<AInfoPlayerState>(allPlayers[_i]->GetController()->PlayerState))
			{
				_playerState->SetCurrentItems(TArray<FItem>());
				LOG("ClearItem");
			}
		}
	}
}

