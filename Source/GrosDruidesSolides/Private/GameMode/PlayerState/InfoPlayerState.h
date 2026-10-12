// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ItemData/FItem.h"
#include "InfoPlayerState.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EPlayerColor : uint8
{
	PC_NONE UMETA(DisplayName = "PC_NONE"),
	PC_BLUE UMETA(DisplayName = "PC_BLUE"),
	PC_RED UMETA(DisplayName = "PC_RED"),
	PC_GREEN UMETA(DisplayName = "PC_GREEN"),
	PC_YELLOW UMETA(DisplayName = "PC_YELLOW")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdatePoint, int, _point);

UCLASS()
class AInfoPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintAssignable) FOnUpdatePoint onUpdatePoint;
protected:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_PlayerColor, BlueprintReadOnly) EPlayerColor playerColor = EPlayerColor::PC_NONE;
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Point, BlueprintReadOnly) int point = 0;
	UPROPERTY(EditAnywhere, Replicated)TArray<FItem> currentItems = TArray<FItem>();
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TMap<EPlayerColor, FLinearColor> colorForPlayer;

public:
	FORCEINLINE int GetPoint() { return point; }
	FORCEINLINE EPlayerColor GetPlayerColor() { return playerColor; }
	FORCEINLINE TArray<FItem> GetCurrentItems() { return currentItems; }

	FORCEINLINE void SetPoint(int _point) { point = _point; onUpdatePoint.Broadcast(point);}
	FORCEINLINE void SetPlayerColor(EPlayerColor _playerColor) { playerColor = _playerColor; if (HasAuthority())UpdatePlayerColor();}
	FORCEINLINE void SetCurrentItems(TArray<FItem> _currentItems) { currentItems = _currentItems; }

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(APlayerState* PlayerState) override;

	UFUNCTION()	void OnRep_Point();
	UFUNCTION()	void OnRep_PlayerColor();


public:
	void UpdatePlayerColor();

	void ResetForLobby();


};
