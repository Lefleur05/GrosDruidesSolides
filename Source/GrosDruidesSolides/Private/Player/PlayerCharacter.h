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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAddItem, APlayerCharacter*, Player, FItem, _item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRespawn, APlayerCharacter*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAddLife, APlayerCharacter*, Player, int, damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScroll, float, scrollValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenItemList);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateUIItem, const TArray<FItem>& , _currentItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateUILife, int, _currentLife, int, _maxLife);

UCLASS()
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnDeath onDeath;
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnAddItem onAddItem;
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnRespawn onRespawn;
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnAddLife onAddLife;
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnScroll onScroll;
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnOpenItemList onOpenItemList;
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnUpdateUIItem onUpdateUIItem;
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnUpdateUILife onUpdateUILife;
private:
	//Component
	UPROPERTY(EditAnywhere) TObjectPtr<UArrowComponent> forwardArrow = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UChildActorComponent> spawnProjectilPoint = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<USpringArmComponent> springArm = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UCameraCustomComponent> cameraCustom = nullptr;

	//Input
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> moveAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> dashAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> scrollAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> openItemListAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> shootPrimarySpellAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> shootSecondarySpellAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> shootShieldSpellAction = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UInputAction> shootUltimatedSpellAction = nullptr;

	//Life
	UPROPERTY(EditAnywhere, Replicated) bool canTakeDamage = true;
	UPROPERTY(EditAnywhere, Replicated) bool isDead = false;
	UPROPERTY(EditAnywhere) float initialMaxLife = 100.0f;

	//Movement
	UPROPERTY(EditAnywhere, Replicated) bool canMove = true;
	UPROPERTY(EditAnywhere) float initialMoveSpeed = 600.0f;	

	//Dash
	UPROPERTY(EditAnywhere) bool canDash = true;
	UPROPERTY(EditAnywhere) float initialCooldownDash = 4.5f;	
	FTimerHandle timerCooldownDash;

	UPROPERTY(EditAnywhere) TObjectPtr<UMaterialInterface> clientMaterial = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UMaterialInterface> clientMaterial2 = nullptr;

protected:

	//Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<USpellComponent> spell = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UCameraComponent> camera = nullptr;

	//Life
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_UI, BlueprintReadOnly) int currentLife = 100.0f;
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_UI, BlueprintReadOnly) int maxLife = 100.0f;

	//Item
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_UI, BlueprintReadOnly)TArray<FItem> currentItems = TArray<FItem>();

	//Dash
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly) float cooldownDash = 4.5f;

	//UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool isInPauseMenu = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool countDownIsStarted = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float currentCountDown = -1.0f;

public:
	FORCEINLINE bool GetIsDead() { return isDead; }
	FORCEINLINE TArray<FItem> GetCurrentItems() { return currentItems; }
	FORCEINLINE TObjectPtr<UChildActorComponent> GetSpawnProjectilPoint() { return spawnProjectilPoint; }
	FORCEINLINE TObjectPtr<USpringArmComponent> GetSpringArm() { return springArm; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetCamera() { return camera; }
	FORCEINLINE TObjectPtr<UCameraCustomComponent> GetCameraCustom() { return cameraCustom; }
	FORCEINLINE TObjectPtr<USpellComponent> GetSpellComponent() { return spell; }
	FORCEINLINE bool GetIsInPauseMenu() { return isInPauseMenu; }

	FORCEINLINE void SetCanMove(bool _canMove) { canMove = _canMove; }
	FORCEINLINE void SetCanDash(bool _canDash) { canDash = _canDash; }
	FORCEINLINE void SetCanTakeDamage(bool _canTakeDamage) { canTakeDamage = _canTakeDamage; }


public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;
	virtual void OnRep_PlayerState() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
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
	UFUNCTION(Client, Reliable) void Client_Death();
	void RagDoll();
	void DisableRagDoll();

	UFUNCTION(Server, Reliable)void Server_Respawn(bool _deleteItem);
	UFUNCTION(NetMulticast, Reliable) void Multi_Respawn(bool _deleteItem);

	void UpdateStatsItem();
	UFUNCTION(Server, Reliable)void Server_UpdateStatsItem();
	UFUNCTION(NetMulticast, Reliable) void Multi_UpdateStatsItem();

	void OpenItemList(const FInputActionValue& _value);
	void ScrollItemList(const FInputActionValue& _value);

	//UI
	UFUNCTION()	void OnRep_UI();
	void UpdateCountDown(float _deltatime);

public:
	void AddLife(const int& _lifeToAdd);
	void AddItem(const FItem& _item);

	void Respawn(bool _deleteItem = true);

	UFUNCTION(BlueprintPure) float GetDashCooldownRemaining();

	UFUNCTION(Client, Reliable) void Client_StartCountDownUI(int _countDown, bool _countDownImmediatlyStart = true);


};
