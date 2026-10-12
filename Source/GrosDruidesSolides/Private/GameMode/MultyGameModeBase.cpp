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
	
	SelectPlayerColor(NewPlayer);

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

	TObjectPtr<AInfoPlayerState> _playerState = Exiting->GetPlayerState<AInfoPlayerState>();
	if (!_playerState)return;

	playerColor.Remove(_playerState->GetPlayerColor());
	
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

void AMultyGameModeBase::SetPlayerLocationAndRotation()
{
	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			if (allSpawnPoint.Num() >= 1.0f)
			{
				int _y = _i % allSpawnPoint.Num();
				_player->SetActorLocation(allSpawnPoint[_y]);

				FRotator _newRot = UKismetMathLibrary::FindLookAtRotation(allSpawnPoint[_y], FVector(0.0f));
				FRotator _playerRot = _player->GetActorRotation();
				_newRot = FRotator(_playerRot.Pitch, _newRot.Yaw, _playerRot.Roll);
				//_player->SetActorRotation(_newRot);

				_player->TeleportTo(allSpawnPoint[_y], _newRot);

				//_player->GetCamera()->SetWorldRotation(_playerRot);
			}
		}
	}
}

void AMultyGameModeBase::StartCountDown(float _countDown, bool _countDownImmediatlyStart)
{
	const int& _count = allPlayers.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		TObjectPtr<APlayerCharacter> _player = allPlayers[_i];
		if (IsValid(_player))
		{
			_player->Client_StartCountDownUI(_countDown, _countDownImmediatlyStart);

		}
	}
}

void AMultyGameModeBase::SelectPlayerColor(TObjectPtr<APlayerController> _newPlayer)
{
	EPlayerColor _newColor = EPlayerColor::PC_NONE;

	if (!playerColor.Contains(EPlayerColor::PC_BLUE))
		_newColor = EPlayerColor::PC_BLUE;
	else if (!playerColor.Contains(EPlayerColor::PC_RED))
		_newColor = EPlayerColor::PC_RED;
	else if (!playerColor.Contains(EPlayerColor::PC_GREEN))
		_newColor = EPlayerColor::PC_GREEN;
	else if (!playerColor.Contains(EPlayerColor::PC_YELLOW))
		_newColor = EPlayerColor::PC_YELLOW;
	//else
		//LOG("PC_NONE Color");

	TObjectPtr<AInfoPlayerState> _playerState = _newPlayer->GetPlayerState< AInfoPlayerState>();
	if (!_playerState)return;

	_playerState->SetPlayerColor(_newColor);
	playerColor.Add(_newColor, _playerState);

}
