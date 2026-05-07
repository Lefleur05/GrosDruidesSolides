// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ItemData/FItem.h"
#include "InfoPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AInfoPlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Replicated) int point = 0;
	UPROPERTY(EditAnywhere, Replicated)TArray<FItem> currentItems = TArray<FItem>();

public:
	FORCEINLINE int GetPoint() { return point; }
	FORCEINLINE TArray<FItem> GetCurrentItems() { return currentItems; }

	FORCEINLINE void SetPoint(int _point) { point = _point; }
	FORCEINLINE void SetCurrentItems(TArray<FItem> _currentItems) { currentItems = _currentItems; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(APlayerState* PlayerState) override;

public:

	void ResetForLobby();


};
