// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData/FItem.h"
#include "SpellProjectil.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class APlayerCharacter;
class UNiagaraSystem;

UCLASS()
class ASpellProjectil : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere) TObjectPtr<USphereComponent> collision = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UStaticMeshComponent> mesh = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UProjectileMovementComponent> movement = nullptr;

	UPROPERTY(EditAnywhere) TObjectPtr<APlayerCharacter> ownerCaster = nullptr;

	//UPROPERTY(EditAnywhere) TObjectPtr<UNiagaraSystem> explosionFX = nullptr;

	UPROPERTY() bool kill =false;

	UPROPERTY(EditAnywhere) float damage = -10.0f;
	UPROPERTY(EditAnywhere) float range = 1500.0f;
	UPROPERTY(EditAnywhere) float knockback = 1.0f;
	UPROPERTY(EditAnywhere) float knockbackMinPowerPercent = 20.0f; // %
	UPROPERTY(EditAnywhere) float explosionSize = 0.0f;
	UPROPERTY(EditAnywhere) float explosionDamage = 0.0f;
	UPROPERTY(EditAnywhere) bool CancelSpell = false;
	UPROPERTY(EditAnywhere) TArray<TEnumAsByte<EObjectTypeQuery>> playerLayer;

	UPROPERTY(EditAnywhere) bool hasCollide = false;
	UPROPERTY(EditAnywhere) bool hasExploded = false;
	UPROPERTY(EditAnywhere) bool hasKnockbacked = false;

	UPROPERTY(Editanywhere) FVector SpawnLocation = FVector(0.0f);
public:
	FORCEINLINE TObjectPtr<USphereComponent> GetCollision() { return collision; }
	FORCEINLINE TObjectPtr<UProjectileMovementComponent> GetProjectilMovement() { return movement; }
	FORCEINLINE TObjectPtr<APlayerCharacter> GetOwnerCaster() { return ownerCaster;}

	FORCEINLINE void SetOwnerCaster(TObjectPtr<APlayerCharacter> _ownerCaster) { ownerCaster = _ownerCaster;}

public:	
	ASpellProjectil();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void SpawnCollide();
	void Collide(TObjectPtr<AActor> _actor);

	UFUNCTION(Server, Reliable) void Server_SetLifeSpan(const float& _time);
	UFUNCTION(NetMulticast, Reliable) void Multi_SetLifeSpan(const float& _time);

	void Explosion();
	UFUNCTION(NetMulticast, Reliable) void Multi_Explosion(const float& _explosionSize);

	void Knockback(TObjectPtr<APlayerCharacter> _player, bool _isExplosionKnockback = false);	

public:
	void SetStatWithItem(FItem _item);

};
