// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "DrainZone.generated.h"

class APlayerCharacter;

UCLASS()
class ADrainZone : public ATriggerBox
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) TObjectPtr<UStaticMeshComponent> mesh = nullptr;
	UPROPERTY(EditAnywhere) TSet<TObjectPtr<APlayerCharacter>> playerInZone = TSet<TObjectPtr<APlayerCharacter>>();
	UPROPERTY(EditAnywhere) float timeForDrain = 0.075f;
	UPROPERTY(EditAnywhere) float lifeToDrain = 1.0f;
	FTimerHandle drainTimer;

public:
	ADrainZone();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION() void Drain();

};
