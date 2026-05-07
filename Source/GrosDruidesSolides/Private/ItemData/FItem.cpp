// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData/FItem.h"

FString FItem::GetItemName()
{
    FString _name = itemName;

	switch (tier)
	{
	case EItemTier::IT_NONE:
		break;
	case EItemTier::IT_TierI:
		_name += " I";
		break;
	case EItemTier::IT_TierII:
		_name += " II";
		break;
	case EItemTier::IT_TierIII:
		_name += " III";
		break;
	case EItemTier::IT_TierIV:
		_name += " IV";
		break;
	default:
		break;
	}

    return _name;
}
