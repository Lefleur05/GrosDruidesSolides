// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/MultyGameModeBase.h"
#include <PlayerController/MultyPlayerController.h>
#include "Player/PlayerCharacter.h"
#include "Utility.h"
#include <Camera/MultyplayerCamera.h>
#include <Kismet/GameplayStatics.h>
#include "Player/Component/CameraCustomComponent.h"

void AMultyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (AMultyPlayerController* _pc = Cast<AMultyPlayerController>(NewPlayer))
	{
		//_pc->ClientSetupEnhancedInput();
		//RequestRespawn(_pc);
	}

	//TObjectPtr<APlayerCharacter> _newPlayer = Cast<APlayerCharacter>(NewPlayer->GetPawn());
	//if (_newPlayer)
	//{
	//	if(!allPlayers.Contains(_newPlayer))
	//		allPlayers.Add(_newPlayer);
	//	if (allSpawnPoint.Num() > 0.0f)
	//	{
	//		const int& _i = allPlayers.Num() % allSpawnPoint.Num();
	//		_newPlayer->SetActorLocation(allSpawnPoint[_i]);
	//	}
	//}
	//LOG("PostLogin allPlayers count => " + FString::FromInt(allPlayers.Num()));
}

void AMultyGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (!Exiting)return;
	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(Exiting->GetPawn());
	if (_player)
	{
		allPlayers.Remove(_player);
	}
	else
	{
		for (int _i = 0; _i < allPlayers.Num(); _i++)
		{
			if (!allPlayers[_i])
				allPlayers.RemoveAt(_i);
		}
	}
}

void AMultyGameModeBase::RequestRespawn(AController* _controller)
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

void AMultyGameModeBase::NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player)
{
	if (!_player) return;
	if (!allPlayers.Contains(_player))
		allPlayers.Add(_player);

	for (TObjectPtr< APlayerCharacter> _playerTemp : allPlayers)
	{
		if(_playerTemp)
		{
			LOG(_playerTemp.GetName() + " => set PlayerList " + FString::FromInt(allPlayers.Num()));
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
	{
		currentCamera = Cast<AMultyplayerCamera>(cameras[0]);
		if (currentCamera)
		{
			//LOG_CUSTOM("Camera Set View Target", Magenta);
			currentCamera->SetPlayerList(allPlayers);
			//_controller->SetViewTarget(currentCamera);

		}
		//else
			//LOG_ERROR("No Camera");
	}
	
}

void AMultyGameModeBase::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	//for (FConstPlayerControllerIterator _it = GetWorld()->GetPlayerControllerIterator(); _it; ++_it)
	//{
	//	TObjectPtr<APlayerController> _playerController = Cast<APlayerController>(_it->Get());
	//	if (_playerController && _playerController->GetPawn() == nullptr)
	//	{
	//		RestartPlayer(_playerController);
	//	}
	//}
}
