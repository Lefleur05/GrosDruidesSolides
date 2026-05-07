// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ItemData/FItem.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"

class ASpellProjectil;
class USpellComponent;
class UCameraCustomComponent;

class USkeletalMeshComponent;
class UInputAction;
class UArrowComponent;
class UChildActorComponent;
class UMaterialInterface;
struct FInputActionValue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, APlayerCharacter*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddItem, APlayerCharacter*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRespawn, APlayerCharacter*, Player);

UCLASS()
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnDeath onDeath;
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnAddItem onAddItem;
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnRespawn onRespawn;
private:

	//Component
	UPROPERTY(EditAnywhere) TObjectPtr<UArrowComponent> forwardArrow = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UChildActorComponent> spawnProjectilPoint = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<USpellComponent> spell = nullptr;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UCameraComponent> camera = nullptr;

private:
	UPROPERTY(EditAnywhere) TObjectPtr<USpringArmComponent> springArm = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UCameraCustomComponent> cameraCustom = nullptr;

	//Input
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> moveAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> dashAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> shootPrimarySpellAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> shootSecondarySpellAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> shootShieldSpellAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> shootUltimatedSpellAction = nullptr;

	//Life
protected:
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly) float currentLife = 100.0f;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly) float maxLife = 100.0f;
private:
	UPROPERTY(EditAnywhere, Replicated) bool canTakeDamage = true;
	UPROPERTY(EditAnywhere, Replicated) bool isDead = false;
	UPROPERTY(EditAnywhere) float initialMaxLife = 100.0f;

	//Movement
	UPROPERTY(EditAnywhere, Replicated) bool canMove = true;
	UPROPERTY(EditAnywhere) float initialMoveSpeed = 600.0f;	

	//Dash
	UPROPERTY(EditAnywhere) bool canDash = true;
	UPROPERTY(EditAnywhere, Replicated) float cooldownDash = 4.5f;	
	UPROPERTY(EditAnywhere) float initialCooldownDash = 4.5f;	
	FTimerHandle timerCooldownDash;

	//Item
	UPROPERTY(EditAnywhere, Replicated)TArray<FItem> currentItems = TArray<FItem>();

	UPROPERTY(EditAnywhere) TObjectPtr<UMaterialInterface> clientMaterial = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UMaterialInterface> clientMaterial2 = nullptr;

public:
	FORCEINLINE bool GetIsDead() { return isDead; }
	FORCEINLINE TArray<FItem> GetCurrentItems() { return currentItems; }
	FORCEINLINE TObjectPtr<UChildActorComponent> GetSpawnProjectilPoint() { return spawnProjectilPoint; }
	FORCEINLINE TObjectPtr<USpringArmComponent> GetSpringArm() { return springArm; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetCamera() { return camera; }
	FORCEINLINE TObjectPtr<UCameraCustomComponent> GetCameraCustom() { return cameraCustom; }
	FORCEINLINE TObjectPtr<USpellComponent> GetSpellComponent() { return spell; }

	FORCEINLINE void SetCanMove(bool _canMove) { canMove = _canMove; }
	FORCEINLINE void SetCanDash(bool _canDash) { canDash = _canDash; }
	FORCEINLINE void SetCanTakeDamage(bool _canTakeDamage) { canTakeDamage = _canTakeDamage; }

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PawnClientRestart() override;
	virtual void PossessedBy(AController* _controller)override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	void Movement(const FInputActionValue& _value);

	void Dash();
	UFUNCTION(Server, Reliable)void Server_Dash();
	UFUNCTION(NetMulticast, Reliable) void Multi_Dash();
	void DashRecover();


	void Death();
	UFUNCTION(Server, Reliable)void Server_Death();
	UFUNCTION(NetMulticast, Reliable) void Multi_Death();
	void RagDoll();
	void DisableRagDoll();

	UFUNCTION(Server, Reliable)void Server_Respawn(bool _deleteItem);
	UFUNCTION(NetMulticast, Reliable) void Multi_Respawn(bool _deleteItem);

	void UpdateStatsItem();
	UFUNCTION(Server, Reliable)void Server_UpdateStatsItem();
	UFUNCTION(NetMulticast, Reliable) void Multi_UpdateStatsItem();

public:
	void AddLife(const float& _lifeToAdd);
	void AddItem(const FItem& _item);

	void Respawn(bool _deleteItem = true);
	
};
