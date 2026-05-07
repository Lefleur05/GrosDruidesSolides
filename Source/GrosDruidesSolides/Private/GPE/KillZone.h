// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "KillZone.generated.h"

UCLASS()
class AKillZone : public ATriggerBox
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) TObjectPtr<UStaticMeshComponent> mesh = nullptr;

public:	
	AKillZone();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

};
