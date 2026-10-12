// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatDummy.generated.h"

class UCapsuleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDummyAddLife, ACombatDummy*, combatDummy, float, damage);

UCLASS()
class ACombatDummy : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnDummyAddLife onDummyAddLife;

	UPROPERTY(EditAnywhere,Replicated, BlueprintReadOnly) float currentLife = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float maxLife = 100.0f;
private:

	UPROPERTY(EditAnywhere) TObjectPtr<UCapsuleComponent> collision = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<USkeletalMeshComponent> mesh = nullptr;


public:	
	ACombatDummy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void AddLife(const float& _lifeToAdd);


};
