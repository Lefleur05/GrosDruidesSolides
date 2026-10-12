// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UPartyGameInstanceSubsystem.generated.h"


UENUM(BlueprintType)
enum class EPartyType : uint8
{
	PT_NONE,
	PT_CLASSIC,
};

/**
 * 
 */
UCLASS()
class UUPartyGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) EPartyType currentPartyType = EPartyType::PT_CLASSIC;

public:
	void SetCurrentPartyType(EPartyType _newPartyType) { currentPartyType = _newPartyType; }
	EPartyType GetCurrentPartyType() { return currentPartyType; }
	
};
