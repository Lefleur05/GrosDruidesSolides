// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerSwitchLevel.generated.h"

class UBoxComponent;
class APlayerCharacter;

UCLASS()
class ATriggerSwitchLevel : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) TObjectPtr<UBoxComponent> collision = nullptr;
	//UPROPERTY(EditAnywhere) int countPlayerInActor = 0;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<APlayerCharacter>> listPlayerContain = TArray<TObjectPtr<APlayerCharacter>>();
	UPROPERTY(EditAnywhere) FString targetLevel;

	UPROPERTY(EditAnywhere) bool clearIteam = true;

	UPROPERTY(EditAnywhere) FTimerHandle timerStartGame;
	UPROPERTY(EditAnywhere) float timeStartGame = 5.0f;
public:	
	ATriggerSwitchLevel();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* _otherActor) override;
	virtual void NotifyActorEndOverlap(AActor* _otherActor) override;

	void StartGame();
	void ClearIteam();
};
