// Fill out your copyright notice in the Description page of Project Settings.


#include "MultyPlayerController.h"
#include <EnhancedInputSubsystems.h>
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include <EnhancedInputLibrary.h>
#include "Utility.h"
#include <GameMode/MultyGameModeBase.h>
#include "Player/PlayerCharacter.h"
#include "GameMode/NewGameMode/AGDSGameMode.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/UserWidget.h"

void AMultyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	LOG_ERROR("PLAYER CONTROLLER ACTIVE");
}

void AMultyPlayerController::ClientRestart_Implementation(APawn* _newPawn)
{
	Super::ClientRestart_Implementation(_newPawn);

	//LOG_SERVER_CLIENT("ClientRestart | NetMode= " + FString::SanitizeFloat((int32)GetNetMode()) + " | IsLocal = " + (IsLocalController() ? "True" : "False"));
	//LOG("ClientRestart_Implementation");
	ClientSetupEnhancedInput(); // if called, called twice in total
}

void AMultyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TObjectPtr<AAGDSGameMode> _newGameMode = GetWorld()->GetAuthGameMode<AAGDSGameMode>();
	if (_newGameMode)
	{
		if (TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(InPawn))
			_newGameMode->NotifyPawnPossessed(this, _player);
		return;
	}

	TObjectPtr<AMultyGameModeBase> _gm = GetWorld()->GetAuthGameMode<AMultyGameModeBase>();
	if (_gm)
	{
		if(TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(InPawn))
			_gm->NotifyPawnPossessed(this, _player);
		return;
	}
}

void AMultyPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	// S'exécute sur le serveur lors d'un Seamless Travel quand le Pawn est ré-assigné
	if (HasAuthority())
	{
		TObjectPtr<AAGDSGameMode> _newGameMode = GetWorld()->GetAuthGameMode<AAGDSGameMode>();
		if (_newGameMode)
		{
			if (TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(P))
				_newGameMode->NotifyPawnPossessed(this, _player);
			return;
		}

		TObjectPtr<AMultyGameModeBase> _gm = GetWorld()->GetAuthGameMode<AMultyGameModeBase>();
		if (_gm)
		{
			if (TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(P))
				_gm->NotifyPawnPossessed(this, _player);
			return;
		}
	}
}

void AMultyPlayerController::Client_InitializeHUD_Implementation(TSubclassOf<UUserWidget> _uiToSpawn)
{
	if (!IsLocalController())return;
	UWidgetLayoutLibrary::RemoveAllWidgets(this);

	if (_uiToSpawn)
	{
		if (UUserWidget* newWidget = CreateWidget<UUserWidget>(this, _uiToSpawn))
		{
			newWidget->AddToViewport();
		}
	}

}

void AMultyPlayerController::ClientSetupEnhancedInput_Implementation()
{
	//LOG("ClientRestart | NetMode= " + FString::SanitizeFloat((int32)GetNetMode()) + " | IsLocal = " + (IsLocalController() ? "True" : "False"));
	//LOG("ClientSetupEnhancedInput_Implementation");
	if (!IsLocalController()) return;
	if (enhancedInputInitialized)
	{
		//LOG("No More Mapping");
		return;
	}

	ULocalPlayer* _localPlayer = GetLocalPlayer();
	//LOG("LocalPlayer " + (_localPlayer ? "VALID" : "NULL"));

	if (!_localPlayer) return;
	enhancedInputInitialized = true;
	UEnhancedInputLocalPlayerSubsystem* _sys = _localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!_sys) return;
	_sys->ClearAllMappings();
	_sys->AddMappingContext(imc, 0);
	//LOG("AddMapping");
	FInputModeGameOnly _inputMode;
	SetInputMode(_inputMode);
	bShowMouseCursor = false;




}