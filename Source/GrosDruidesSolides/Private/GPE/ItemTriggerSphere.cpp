// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/ItemTriggerSphere.h"
#include "Player/PlayerCharacter.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "Utility.h"
#include "Components/ShapeComponent.h"
#include "DataAsset/AllItemsDataAsset.h"
#include <Net/UnrealNetwork.h>
#include "GameAnalytics.h"
#include "GameAnalyticsModule.h"

AItemTriggerSphere::AItemTriggerSphere()
{
	PrimaryActorTick.bCanEverTick = true;
	//RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	spriteComp = CreateDefaultSubobject<UPaperSpriteComponent>("SpriteComp");
	flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("flipbook");
	spriteComp->SetupAttachment(RootComponent);
	flipbook->SetupAttachment(RootComponent);
}

void AItemTriggerSphere::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() && GetCollisionComponent())
	{
		GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AItemTriggerSphere::OnOverlapBegin);
	}

	

	if (HasAuthority())
		UpdateItem();
	else
		OnRep_Item();


}

void AItemTriggerSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	OrientToCamera();
}

void AItemTriggerSphere::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AItemTriggerSphere, item, COND_None,REPNOTIFY_Always);
}

void AItemTriggerSphere::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(OtherActor);
	if (!_player)return;

	bool _canTakeItem = true;
	onTryTake.Broadcast(_canTakeItem,_player);

	if (!_canTakeItem)return;

	AddItemToPlayerDetected(_player);
}

void AItemTriggerSphere::OnRep_Item()
{
	//LOG_CUSTOM("Rep" + item.itemName, Silver);
	UPaperSprite* _sprite = item.sprite.IsValid() ? item.sprite.Get(): item.sprite.LoadSynchronous();

	UPaperFlipbook* _flipbook = item.flipbook.IsValid()	? item.flipbook.Get(): item.flipbook.LoadSynchronous();

	//LOG_CUSTOM("OnRep_Item name=" + item.itemName + " sprite = "+ (_sprite ?"VALID" : "NULL") + " flipbook =" + (_flipbook ? "VALID" : "NULL"), Purple);
	if (_sprite) spriteComp->SetSprite(_sprite);
	if (_flipbook) flipbook->SetFlipbook(_flipbook);

	onUpdateItem.Broadcast(item);
}

void AItemTriggerSphere::AddItemToPlayerDetected(TObjectPtr<APlayerCharacter> _player)
{
	_player->AddItem(item);
	Destroy();

	if (HasAuthority())
	{
		TObjectPtr<UGameAnalytics> _gameAnalytics = FGameAnalyticsModule::Get().GetInstance();
		_gameAnalytics->AddDesignEvent(
			FString::Printf(TEXT("carrousel:item_pris:%s"), *(item.itemName == "Fléche" ? "Fleche" : item.itemName))
		);
	}
	Multi_AddItemToPlayerDetected(_player);
}

void AItemTriggerSphere::UpdateItem()
{
	UPaperSprite* _sprite = item.sprite.IsValid()
		? item.sprite.Get()
		: item.sprite.LoadSynchronous();

	UPaperFlipbook* _flipbook = item.flipbook.IsValid()
		? item.flipbook.Get()
		: item.flipbook.LoadSynchronous();

	if (_sprite) spriteComp->SetSprite(_sprite);
	if (_flipbook) flipbook->SetFlipbook(_flipbook);

	onUpdateItem.Broadcast(item);

	if (HasAuthority())
	{
		TObjectPtr<UGameAnalytics> _gameAnalytics = FGameAnalyticsModule::Get().GetInstance();
		_gameAnalytics->AddDesignEvent(
			FString::Printf(TEXT("carrousel:item_apparu:%s"), *(item.itemName == "Fléche" ? "Fleche" : item.itemName))
		);
	}
}

void AItemTriggerSphere::OrientToCamera()
{
	if (!IsValid(controller))
	{
		if(GetWorld())
			controller = GetWorld()->GetFirstPlayerController();
	}
	if (IsValid(controller))
	{
		FVector _cameraLocation;
		FRotator _cameraRotation;
		controller->GetPlayerViewPoint(_cameraLocation, _cameraRotation);
		FRotator _newRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), _cameraLocation);
		SetActorRotation(_newRot);
	}
	else
	{
		LOG_ERROR("No Controller For Item OrientToCamera");
	}
}

void AItemTriggerSphere::Multi_AddItemToPlayerDetected_Implementation(APlayerCharacter* _player)
{
	
}
