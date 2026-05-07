// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blades.generated.h"

class URotatingMovementComponent;
class UCapsuleComponent;
class APlayerCharacter;

UCLASS()
class ABlades : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) TObjectPtr<UStaticMeshComponent> bladeOne = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UStaticMeshComponent> bladeTwo = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UStaticMeshComponent> bladeTree = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UStaticMeshComponent> bladeFour = nullptr;

	UPROPERTY(EditAnywhere) TObjectPtr<UCapsuleComponent> capsuleCollision = nullptr;

	UPROPERTY(EditAnywhere) TObjectPtr<URotatingMovementComponent> movement = nullptr;

	UPROPERTY(EditAnywhere) TSet<TObjectPtr<APlayerCharacter>> players = TSet<TObjectPtr<APlayerCharacter>>();
	UPROPERTY(EditAnywhere) float damagePerBlade = 5.0f;
	UPROPERTY(EditAnywhere) float timeDamageLoop = 0.2f;
	FTimerHandle damageTimer;


	
public:	
	ABlades();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
	void Damage();
};
