// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData/FItem.h"
#include "Carrousel.generated.h"

class UAllItemsDataAsset;
class AItemTriggerSphere;
class APlayerCharacter;
class URotatingMovementComponent;

UCLASS()
class ACarrousel : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) TObjectPtr<URotatingMovementComponent> movement = nullptr;


	UPROPERTY(EditAnywhere) TSubclassOf<AItemTriggerSphere> itemToSpawn = nullptr;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<AItemTriggerSphere>> allItems = TArray<TObjectPtr<AItemTriggerSphere>>();
	UPROPERTY(EditAnywhere)	TObjectPtr<UAllItemsDataAsset> itemsDataAsset = nullptr;

	UPROPERTY(EditAnywhere, replicated)	FRotator currentServerRotation = FRotator(0.0f);
	UPROPERTY(EditAnywhere)	FRotator rotationSpeed = FRotator(0.0f);
	UPROPERTY(EditAnywhere)	TArray<FVector> itemsLocation = TArray<FVector>();

	UPROPERTY(EditAnywhere) int countItemToSpawn = 5;

	UPROPERTY(EditAnywhere) int countItemPerPlayer = 1;
	UPROPERTY(EditAnywhere) TMap<APlayerCharacter*,int32> countItemByPlayer = TMap<APlayerCharacter*, int32>();

	UPROPERTY(EditAnywhere) float tier1randomValue = 60.0f;
	UPROPERTY(EditAnywhere) float tier2randomValue = 25.0f;
	UPROPERTY(EditAnywhere) float tier3randomValue = 10.0f;
	UPROPERTY(EditAnywhere) float tier4randomValue = 5.0f;
	
public:	
	ACarrousel();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	void SpawnItem();
	TArray<FItem> GetRandomItem(const int& _itemCount);

	void RotateItemWithCarrousel();
	UFUNCTION(Server, Reliable) void Server_RotateItemWithCarrousel(FRotator _newRot);
	UFUNCTION(NetMulticast, Reliable) void Multi_RotateItemWithCarrousel(FRotator _newRot);

	UFUNCTION() void ObjectTake(bool& _canTakeItem, APlayerCharacter* _player);
	FItem GetNewtRandomItem(FItem _item,TArray<FItem> _itemSpawn, TArray<FItem> _allItemsList, int _itemIndex);
};
