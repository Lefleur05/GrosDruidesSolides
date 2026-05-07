// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerSphere.h"
#include "ItemData/FItem.h"
#include "ItemTriggerSphere.generated.h"

/**
 * 
 */
class UPaperFlipbookComponent;
class UPaperSpriteComponent;
class APlayerCharacter;

UCLASS()
class AItemTriggerSphere : public ATriggerSphere
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTryTake, bool&, _canTakeItem, APlayerCharacter*, _player);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateItem, FItem, _newItem);
public:
	FOnTryTake onTryTake;
	UPROPERTY(EditAnywhere, BlueprintAssignable)FOnUpdateItem onUpdateItem;
private:

	UPROPERTY(EditAnywhere)TObjectPtr<UPaperSpriteComponent> spriteComp = nullptr;
	UPROPERTY(EditAnywhere)TObjectPtr<UPaperFlipbookComponent> flipbook = nullptr;

	UPROPERTY(EditAnywhere)TObjectPtr<APlayerController> controller = nullptr;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Item) FItem item;

public:
	FORCEINLINE void SetItem(FItem _item) { item = _item; }
	FORCEINLINE FItem GetItem() { return item; }

public:
	AItemTriggerSphere();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION() void OnRep_Item();

	UFUNCTION(Server, Reliable) void Server_AddItemToPlayerDetected(APlayerCharacter* _player);
	UFUNCTION(NetMulticast, Reliable) void Multi_AddItemToPlayerDetected(APlayerCharacter* _player);

	void UpdateItem();
	void OrientToCamera();
};
