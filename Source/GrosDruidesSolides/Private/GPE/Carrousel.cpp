// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/Carrousel.h"
#include "GPE/ItemTriggerSphere.h"
#include "DataAsset/AllItemsDataAsset.h"
#include "Utility.h"
#include "Player/PlayerCharacter.h"
#include <Net/UnrealNetwork.h>
#include <GameFramework/RotatingMovementComponent.h>

// Sets default values
ACarrousel::ACarrousel()
{
	PrimaryActorTick.bCanEverTick = true;
	movement = CreateDefaultSubobject<URotatingMovementComponent>("Movement");
	AddOwnedComponent(movement);
}

void ACarrousel::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
		SpawnItem();
}

void ACarrousel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*if (HasAuthority())
		RotateItemWithCarrousel();
	else
	{
		FRotator _smoothedRot = FMath::RInterpTo(GetActorRotation(),currentServerRotation,DeltaTime,15.f);
		SetActorRotation(_smoothedRot);
	}*/
}

void ACarrousel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACarrousel, currentServerRotation);
}

void ACarrousel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	const int& _count = allItems.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if(IsValid(allItems[_i]))
			allItems[_i]->Destroy();
	}
}

void ACarrousel::SpawnItem()
{
	TArray<FItem> _items = GetRandomItem(countItemToSpawn);
	LOG("Count ITEMS " + FString::SanitizeFloat(_items.Num()));
	for (int _i = 0; _i < countItemToSpawn; _i++)
	{
		// SpawnActorDeferred for spawn without beginPlay
		TObjectPtr<AItemTriggerSphere> _item = WORLD->SpawnActorDeferred<AItemTriggerSphere>(itemToSpawn, GetTransform());

		// need to be in Beginplay
		if (_items.Num() > _i) 
			_item->SetItem(_items[_i]);

		_item->FinishSpawning(GetTransform());

		_item->onTryTake.AddDynamic(this, &ACarrousel::ObjectTake);

		allItems.Add(_item);
		_item->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true));
		if (itemsLocation.Num() > _i)
			_item->SetActorLocation(itemsLocation[_i] + GetActorLocation());
	}
}

TArray<FItem> ACarrousel::GetRandomItem(const int& _itemCount)
{
	if (!itemsDataAsset)return TArray<FItem>();

	TArray<FItem> _items = TArray<FItem>();

	for (int _i = 0; _i < _itemCount; _i++)
	{
		int _rand = UKismetMathLibrary::RandomIntegerInRange(0, tier1randomValue + tier2randomValue + tier3randomValue + tier4randomValue);
		if (_rand >= 0.0f && _rand < tier1randomValue && itemsDataAsset->itemsTier1.Num() > 0.0f)
		{
			_rand = FMath::FRandRange(0.0f, itemsDataAsset->itemsTier1.Num()-1);
			FItem _newItem = itemsDataAsset->itemsTier1[_rand];
			if(_items.Contains(_newItem))
				_newItem = GetNewtRandomItem(_newItem, _items, itemsDataAsset->itemsTier1, _rand);
			_items.Add(_newItem);
		}
		else if(_rand >= tier1randomValue && _rand < (tier1randomValue+tier2randomValue) && itemsDataAsset->itemsTier2.Num() > 0.0f)
		{
			_rand = FMath::FRandRange(0.0f, itemsDataAsset->itemsTier2.Num()-1);
			FItem _newItem = itemsDataAsset->itemsTier2[_rand];

			FString _itemName1 = _newItem.itemName;
			int _index = _itemName1.Find(" ");
			_itemName1 = (_index != INDEX_NONE) ? _itemName1.Left(_index) : _itemName1;
			LOG("ItemName Normaliser => " + _itemName1);

			if (_items.Contains(_newItem))
				_newItem = GetNewtRandomItem(_newItem, _items, itemsDataAsset->itemsTier2, _rand);
			_items.Add(_newItem);
		}
		else if (_rand >= (tier1randomValue + tier2randomValue) && _rand < (tier1randomValue + tier2randomValue+ tier3randomValue) && itemsDataAsset->itemsTier3.Num() > 0.0f)
		{
			_rand = FMath::FRandRange(0.0f, itemsDataAsset->itemsTier3.Num()-1);
			FItem _newItem = itemsDataAsset->itemsTier3[_rand];
			if (_items.Contains(_newItem))
				_newItem = GetNewtRandomItem(_newItem, _items, itemsDataAsset->itemsTier3, _rand);
			_items.Add(_newItem);
		}
		else if (_rand >= (tier1randomValue + tier2randomValue + tier3randomValue) && _rand <= (tier1randomValue + tier2randomValue + tier3randomValue + tier3randomValue) && itemsDataAsset->itemsTier4.Num() > 0.0f)
		{
			_rand = FMath::FRandRange(0.0f, itemsDataAsset->itemsTier4.Num()-1);
			FItem _newItem = itemsDataAsset->itemsTier4[_rand];
			if (_items.Contains(_newItem))
				_newItem = GetNewtRandomItem(_newItem, _items, itemsDataAsset->itemsTier4, _rand);
			_items.Add(_newItem);
		}
	}
	
	return _items;
}

void ACarrousel::RotateItemWithCarrousel()
{
	FRotator _newRot = GetActorRotation() + rotationSpeed * DELTATIME;
	SetActorRotation(_newRot);
	currentServerRotation = _newRot;
	/*if (HasAuthority())
		Multi_RotateItemWithCarrousel(_newRot);
	else
		Server_RotateItemWithCarrousel(_newRot);*/
}

void ACarrousel::ObjectTake(bool& _canTakeItem, APlayerCharacter* _player)
{
	_canTakeItem = true;
	if (countItemPerPlayer <= 0 || countItemByPlayer.Find(_player) && countItemByPlayer[_player] >= countItemPerPlayer)
	{
		_canTakeItem = false;
		return;
	}

	if (countItemByPlayer.Find(_player))
		countItemByPlayer[_player]++;
	else
		countItemByPlayer.Add(_player, 1);

}

FItem ACarrousel::GetNewtRandomItem(FItem _item, TArray<FItem> _itemSpawn, TArray<FItem> _allItemsList, int _itemIndex)
{
	int _tryCount = 20.0f;
	while (_itemSpawn.Contains(_item) && _tryCount >0.0f)
	{
		_itemIndex ++;
		_tryCount--;
		_itemIndex = _itemIndex % _allItemsList.Num();
		_item = _allItemsList[_itemIndex];
	}
	return _item;
}

void ACarrousel::Server_RotateItemWithCarrousel_Implementation(FRotator _newRot)
{
	Multi_RotateItemWithCarrousel(_newRot);
}

void ACarrousel::Multi_RotateItemWithCarrousel_Implementation(FRotator _newRot)
{
	//LOG_SERVER_CLIENT(_newRot.ToString());
	SetActorRotation(_newRot);
}

