// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData/FItem.h"
#include "AllItemsDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class UAllItemsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) TArray<FItem> itemsTier1 = TArray<FItem>();
	UPROPERTY(EditAnywhere) TArray<FItem> itemsTier2 = TArray<FItem>();
	UPROPERTY(EditAnywhere) TArray<FItem> itemsTier3 = TArray<FItem>();
	UPROPERTY(EditAnywhere) TArray<FItem> itemsTier4 = TArray<FItem>();


};
