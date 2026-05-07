// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/KillZone.h"
#include <Player/PlayerCharacter.h>

// Sets default values
AKillZone::AKillZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AKillZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKillZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKillZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!HasAuthority())return;
	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(OtherActor);
	if (_player)
	{
		_player->AddLife(-100000.0f);
	}

}

void AKillZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

}

