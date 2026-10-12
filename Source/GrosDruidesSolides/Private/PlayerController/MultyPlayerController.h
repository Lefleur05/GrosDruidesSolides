// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "MultyPlayerController.generated.h"

class UInputMappingContext;

/**
 * 
 */
UCLASS()
class AMultyPlayerController : public APlayerController
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> imc = nullptr;

	UPROPERTY(EditAnywhere)
	bool enhancedInputInitialized = false;

protected:
	virtual void BeginPlay() override;
	virtual void ClientRestart_Implementation(APawn* _newPawn) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void AcknowledgePossession(APawn* P) override;

public:
	UFUNCTION(Client, Reliable)void Client_InitializeHUD(TSubclassOf<UUserWidget> _uiToSpawn);
	UFUNCTION(Client,Unreliable)void ClientSetupEnhancedInput();
	
};
