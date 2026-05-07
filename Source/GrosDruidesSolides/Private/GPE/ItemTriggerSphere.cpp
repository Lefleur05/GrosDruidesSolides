// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/ItemTriggerSphere.h"
#include "Player/PlayerCharacter.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "Utility.h"
#include "Components/ShapeComponent.h"
#include "DataAsset/AllItemsDataAsset.h"
#include <Net/UnrealNetwork.h>

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

	UpdateItem();
}

void AItemTriggerSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	OrientToCamera();
}

void AItemTriggerSphere::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemTriggerSphere, item);
}

void AItemTriggerSphere::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(OtherActor);
	if (!_player)return;

	bool _canTakeItem = true;
	onTryTake.Broadcast(_canTakeItem,_player);

	if (!_canTakeItem)return;

	if (HasAuthority())
		Multi_AddItemToPlayerDetected(_player);
	else
		Server_AddItemToPlayerDetected(_player);
}

void AItemTriggerSphere::OnRep_Item()
{
	LOG_CUSTOM("Rep" + item.itemName, Silver);
	if (item.flipbook)
		flipbook->SetFlipbook(item.flipbook);
	if (item.sprite)
		spriteComp->SetSprite(item.sprite);
	onUpdateItem.Broadcast(item);
}

void AItemTriggerSphere::UpdateItem()
{
	if (item.flipbook)
		flipbook->SetFlipbook(item.flipbook);
	if (item.sprite)
		spriteComp->SetSprite(item.sprite);
	onUpdateItem.Broadcast(item);
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

void AItemTriggerSphere::Server_AddItemToPlayerDetected_Implementation(APlayerCharacter* _player)
{
	Multi_AddItemToPlayerDetected(_player);
}

void AItemTriggerSphere::Multi_AddItemToPlayerDetected_Implementation(APlayerCharacter* _player)
{
	_player->AddItem(item);
	Destroy();
}
