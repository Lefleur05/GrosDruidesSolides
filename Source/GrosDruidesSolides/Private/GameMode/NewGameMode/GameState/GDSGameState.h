// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GDSGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerArrayUpdateGDS);

UCLASS()
class AGDSGameState : public AGameState
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnPlayerArrayUpdateGDS onPlayerArrayUpdateGDS;

protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
};
