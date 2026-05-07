// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/CameraParamDataAsset.h"
#include "CameraCustomComponent.generated.h"

class APlayerCharacter;
class USpringArmComponent;
class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCameraCustomComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) TObjectPtr<APlayerCharacter> player = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<USpringArmComponent> springArm = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UCameraComponent> camera = nullptr;

	UPROPERTY(Editanywhere) TObjectPtr<UCameraParamDataAsset> dataAsset = nullptr;

	UPROPERTY(Editanywhere, Replicated) FCameraParam param;
	
	UPROPERTY(EditAnywhere, Replicated) TArray<TObjectPtr<APlayerCharacter>> playerList = TArray<TObjectPtr<APlayerCharacter>>();

public:
	FORCEINLINE void SetPlayerList(const TArray<TObjectPtr<APlayerCharacter>>& _playerList) { playerList = _playerList; }

public:	
	UCameraCustomComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CameraLocationWithAverage();
	void CameraZoom();

		
};
