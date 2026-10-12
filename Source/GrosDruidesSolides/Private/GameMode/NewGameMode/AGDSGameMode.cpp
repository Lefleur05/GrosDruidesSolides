// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NewGameMode/AGDSGameMode.h"
#include <PlayerController/MultyPlayerController.h>
#include "Player/PlayerCharacter.h"
#include "Utility.h"
#include <Camera/MultyplayerCamera.h>
#include <Kismet/GameplayStatics.h>
#include "Player/Component/CameraCustomComponent.h"
#include "Utility.h"
#include "GameMode/NewGameMode/PartyManager/PartyManager.h"
#include <GameInstance/InfoPlayerGameInstance.h>
#include "GameMode/NewGameMode/PartyManager/PartyManager.h"
#include "GameMode/NewGameMode/MapConfig.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/UPartyGameInstanceSubsystem.h"

void AAGDSGameMode::BeginPlay()
{
	Super::BeginPlay();
	TObjectPtr<AActor> actorMapConfig = UGameplayStatics::GetActorOfClass(GetWorld(), AMapConfig::StaticClass());
	mapConfig = Cast<AMapConfig>(actorMapConfig);
	SpawnPartyManager();
}

/// <summary>
/// Called when a player successfully joins the server.
/// Assigns a unique color to the new player.
/// </summary>
/// <param name="NewPlayer"></param>
void AAGDSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	SelectPlayerColor(NewPlayer);

	/*if (AMultyPlayerController* _pc = Cast<AMultyPlayerController>(NewPlayer))
	{
		//_pc->ClientSetupEnhancedInput();
		//RequestRespawn(_pc);
	}*/
}
void AAGDSGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	playerJoin++;
	allPlayerJoinSession = playerJoin >= 2;
	LOG_ERROR("playerJoin =  " + FString::FromInt(playerJoin));

	Super::PostSeamlessTravel();

	
}

/// <summary>
/// Called when a player leaves the server.
///	Removes the player from all tracked collections.
/// </summary>
/// <param name="Exiting"></param>
void AAGDSGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (!Exiting)return;

	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(Exiting->GetPawn());
	if (_player)
	{
		allPlayers.Remove(_player);
	}
	allPlayers.RemoveAll([](const TObjectPtr<APlayerCharacter>& Player){ return !IsValid(Player); } );


	TObjectPtr<AInfoPlayerState> _playerState = Exiting->GetPlayerState<AInfoPlayerState>();
	if (!_playerState)return;
	playerColor.Remove(_playerState->GetPlayerColor());

}

/// <summary>
/// Respawns a controller after a short delay.
/// </summary>
/// <param name="_controller"></param>
void AAGDSGameMode::RequestRespawn(AController* _controller)
{
	if (!_controller) return;

	FTimerHandle _respawnTimer;
	GetWorldTimerManager().SetTimer(_respawnTimer, FTimerDelegate::CreateLambda([this, _controller]
		{
			// Restart Player will spawn a new pawn and posses it
			RestartPlayer(_controller);
		}),
		3.0f, // Respawn delay
		false);
}

/// <summary>
/// Registers a newly possessed pawn and updates
/// all camera systems with the current player list.
/// </summary>
/// <param name="_playerController"></param>
/// <param name="_player"></param>
void AAGDSGameMode::NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player)
{
	if (!_player) return;
	if (!allPlayers.Contains(_player))
		allPlayers.Add(_player);
	if (!_playerController) return;
	UE_LOG(LogTemp, Warning, TEXT("========== POST SEAMLESS TRAVEL =========="));

	int32 PCCount = 0;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PC[%d] = %s | PlayerState = %s | Pawn = %s"),
			PCCount,
			*GetNameSafe(PC),
			*GetNameSafe(PC->PlayerState),
			*GetNameSafe(PC->GetPawn())
		);

		PCCount++;
	}

	UE_LOG(LogTemp, Warning, TEXT("TOTAL PC = %d"), PCCount);


	TObjectPtr<APlayerController> _controller = Cast<APlayerController>(_playerController);

	UpdateCameraWithNewPlayer(_playerController, _player);
	bool _allplayerconnected = false;
	if (TObjectPtr<UInfoPlayerGameInstance> _gameInstance = GetGameInstance<UInfoPlayerGameInstance>())
	{
		_allplayerconnected = allPlayers.Num() == _gameInstance->GetLobbyPlayerCount();
		LOG("All Player Connected = " + FString::FromInt(allPlayers.Num()) + " == " + FString::FromInt(_gameInstance->GetLobbyPlayerCount()));
	}
	if (_allplayerconnected && WORLD && HasAuthority())
	{
		if (!currentPartyType)
		{
			LOG_ERROR("No currentPartyType aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
			return;
		}
		currentPartyType->SpawnUI();
		currentPartyType->InitParty(allPlayers);
		currentPartyType->StartParty();
	}
}

//TODO search why set 2 time
void AAGDSGameMode::UpdateCameraWithNewPlayer(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player)
{
	if (!_player) return;
	for (TObjectPtr< APlayerCharacter> _playerTemp : allPlayers)
	{
		if (_playerTemp)
		{
			if (_playerTemp->GetCameraCustom())
			{
				LOG(_playerTemp.GetName() + " => set PlayerList " + FString::FromInt(allPlayers.Num()));
				_playerTemp->GetCameraCustom()->SetPlayerList(allPlayers);
			}
		}
	}

	if (!_playerController) return;
	TObjectPtr<APlayerController> _controller = Cast<APlayerController>(_playerController);
	if (!_controller)return;

	TArray<AActor*> cameras;
	UGameplayStatics::GetAllActorsOfClass(WORLD, AMultyplayerCamera::StaticClass(), cameras);
	if (cameras.Num() > 0)
		currentCamera = Cast<AMultyplayerCamera>(cameras[0]);

	if (currentCamera)
	{
		currentCamera->SetPlayerList(allPlayers);
	}
	else
		LOG_ERROR("No Camera");
}

/// <summary>
/// Teleports all tracked players to the configured spawn points.
/// </summary>
void AAGDSGameMode::SetPlayerLocationAndRotation()
{
	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			if (mapConfig)
			{
				TArray<TObjectPtr<AActor>> _allSpawnPoint = mapConfig->GetSpawnTransform();
				if (_allSpawnPoint.Num() > 0)
				{
					int spawnIndex = _i % _allSpawnPoint.Num();
					_player->TeleportTo(_allSpawnPoint[spawnIndex]->GetActorLocation(), _allSpawnPoint[spawnIndex]->GetActorRotation());
				}
			}
		}
	}
}

/// <summary>
/// Starts a countdown UI on all connected players.
/// </summary>
/// <param name="_countDown"></param>
/// <param name="_countDownImmediatlyStart"></param>
void AAGDSGameMode::StartCountDown(float _countDown, bool _countDownImmediatlyStart)
{
	for (TObjectPtr<APlayerCharacter> _player : allPlayers)
	{
		if (IsValid(_player))
		{
			_player->Client_StartCountDownUI(_countDown, _countDownImmediatlyStart);
		}
	}
}

/// <summary>
/// Assigns the first available player color
/// to the specified controller.
/// </summary>
/// <param name="_newPlayer"></param>
void AAGDSGameMode::SelectPlayerColor(TObjectPtr<APlayerController> _newPlayer)
{
	EPlayerColor _newColor = EPlayerColor::PC_NONE;

	const TArray<EPlayerColor> AvailableColors =
	{
		EPlayerColor::PC_BLUE,
		EPlayerColor::PC_RED,
		EPlayerColor::PC_GREEN,
		EPlayerColor::PC_YELLOW
	};

	for (EPlayerColor Color : AvailableColors)
	{
		if (!playerColor.Contains(Color))
		{
			_newColor = Color;
			break;
		}
	}

	TObjectPtr<AInfoPlayerState> _playerState = _newPlayer->GetPlayerState<AInfoPlayerState>();
	if (!_playerState)return;

	_playerState->SetPlayerColor(_newColor);
	playerColor.Add(_newColor, _playerState);

}

void AAGDSGameMode::SpawnPartyManager()
{
	LOG_ERROR("SpawnPartyManager");
	if (!WORLD)return;
	if (TObjectPtr<UUPartyGameInstanceSubsystem> _gameInstance = GetGameInstance<UGameInstance>()->GetSubsystem<UUPartyGameInstanceSubsystem>())
	{
		FString _type = StaticEnum<EPartyType>()->GetNameStringByValue(static_cast<int64>(_gameInstance->GetCurrentPartyType()));
		LOG_ERROR("SpawnPartyManager Party Type" + _type);
		TSubclassOf<APartyManager> _actorToSpawn = allPartyType[_gameInstance->GetCurrentPartyType()];
		currentPartyType = WORLD->SpawnActorDeferred<APartyManager>(_actorToSpawn,FTransform::Identity);
		currentPartyType->SetGameMode(this);
		UGameplayStatics::FinishSpawningActor(currentPartyType, FTransform::Identity);
		if(!currentPartyType)
		{
			LOG_ERROR("currentPartyType not spawn aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
			return;
		}
		LOG("spawnPartyManager a&aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" + currentPartyType.GetName());
	}
	else
		LOG_ERROR("No UUPartyGameInstanceSubsystem in SpawnPartyManager");
}


TArray<TObjectPtr<APlayerCharacter>> AAGDSGameMode::SpawnAllPlayers(const TArray<TObjectPtr<APlayerController>>& _controllers)
{
	allPlayers.Empty();
	TArray<TObjectPtr<AActor>> _spawnPoints = mapConfig->GetSpawnTransform();

	UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers called with %d controllers"), _controllers.Num());
	for (int _i = 0; _i < _controllers.Num(); _i++)
	{
		UE_LOG(LogTemp, Warning, TEXT(" - %s"), *GetNameSafe(_controllers[_i]));

		TObjectPtr<APlayerController> _pc = _controllers[_i];
		if (!_pc) continue;

		FTransform _spawnTransform = _spawnPoints[_i % _spawnPoints.Num()]->GetActorTransform();

		TObjectPtr<APlayerCharacter> _newPawn = GetWorld()->SpawnActor<APlayerCharacter>(playerCharacterClass, _spawnTransform);

		if (_newPawn)
		{
			_pc->Possess(_newPawn); // Déclenche PossessedBy / OnPossess / ClientRestart proprement
			if (!allPlayers.Contains(_newPawn))
				allPlayers.Add(_newPawn);
		}
	}
	return allPlayers;
}
