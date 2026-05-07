// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include <PaperFlipbook.h>
#include <Utility.h>
#include "FItem.generated.h"

UENUM(BlueprintType)
enum class EItemTier : uint8
{
	IT_NONE UMETA(DisplayName = "IT_NONE"),
	IT_TierI UMETA(DisplayName = "IT_TierI"),
	IT_TierII UMETA(DisplayName = "IT_TierII"),
	IT_TierIII UMETA(DisplayName = "IT_TierIII"),
	IT_TierIV UMETA(DisplayName = "IT_TierIV"),

};

UENUM(BlueprintType)
enum class ESpellType : uint8
{
	ST_MINOR_PROJECTILE UMETA(DisplayName = "ST_MINOR_PROJECTILE"),
	ST_SECONDARY_PROJECTILE UMETA(DisplayName = "ST_SECONDARY_PROJECTILE"),
	ST_SHIELD_PROJECTILE UMETA(DisplayName = "ST_SHIELD_PROJECTILE"),
	ST_ULTIMATE_PROJECTILE UMETA(DisplayName = "ST_ULTIMATE_PROJECTILE"),

};

USTRUCT(BlueprintType)
struct FItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString itemName = "item";
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UPaperSprite> sprite;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UPaperFlipbook> flipbook;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString itemDescription = "This is an item";
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EItemTier tier = EItemTier::IT_TierI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ESpellType spell = ESpellType::ST_MINOR_PROJECTILE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int projectilNumber = 0;									//In Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float damage = 0.0f;										//In projectil
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float projectilSpeed = 0.0f;								//In projectil
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float cooldown = 1.0f;// in Percent							//In Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float castTime = 1.0f;// in Percent							//In Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector size = FVector(0.0f);								//In projectil
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float range = 0.0f;											//In projectil
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float knockback = 0.0f;										//In projectil
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float explosionSize = 0.0f;									//In projectil
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float explosionDamage = 0.0f;								//In projectil
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float coneSize = 0.0f;										//In Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool CancelSpell = false;									//In projectil
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float hp = 0.0f;											//Player Modification
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float speed = 0.0f;											//Player Modification
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float dashColldown = 1.0f;									//Player Modification
	
	FString GetItemName();

	FItem operator += (const FItem& _other)
	{
		projectilNumber += _other.projectilNumber;										//In Player
		damage += _other.damage;														//In projectil
		projectilSpeed += _other.projectilSpeed;										//In projectil
		cooldown *= _other.cooldown;													//In Player
		castTime *= _other.castTime;													//In Player
		size += _other.size;															//In projectil
		range += _other.range;															//In projectil
		knockback += _other.knockback;													//In projectil
		explosionSize += _other.explosionSize;											//In projectil
		explosionDamage += _other.explosionDamage;										//In projectil
		coneSize += _other.coneSize;													//In Player
		CancelSpell = _other.CancelSpell ? _other.CancelSpell : CancelSpell;			//In projectil
		return *this;
	}

	bool operator == (const FItem& _other) const
	{
		//Compare the first word and ignore the tier number in the object's name
		//FString _itemName1 = itemName;
		//int _index = _itemName1.Find(" ");
		//_itemName1 = (_index != INDEX_NONE) ? _itemName1.Left(_index) : _itemName1; // (_index != INDEX_NONE) => to find out if he found it
		//
		//FString _itemName2 = _other.itemName;
		//_index = _itemName2.Find(" ");
		//_itemName2 = (_index != INDEX_NONE) ? _itemName2.Left(_index) : _itemName2;
		//if (_itemName1 != _itemName2)return false;

		if (itemName != _other.itemName)return false;
		//if (tier != _other.tier)return false;
		if (spell != _other.spell)return false;
		return true;
	}
};
