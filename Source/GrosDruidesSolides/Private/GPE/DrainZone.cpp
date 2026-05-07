// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/DrainZone.h"
#include <Player/PlayerCharacter.h>

// Sets default values
ADrainZone::ADrainZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADrainZone::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())return;
	TIMER_LOOP(this, drainTimer, Drain(), timeForDrain, timeForDrain);
}

// Called every frame
void ADrainZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADrainZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!HasAuthority())return;
	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(OtherActor);
	if (_player)
	{
		playerInZone.Add(_player);
	}
}

void ADrainZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (!HasAuthority())return;
	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(OtherActor);
	if (_player)
	{
		playerInZone.Remove(_player);
	}

}

void ADrainZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	PAUSE_TIMER(drainTimer);
	CLEAR_TIMER(drainTimer);
}

void ADrainZone::Drain()
{
	//LOG("Drain");
	if (!HasAuthority())return;

	TArray<TObjectPtr<APlayerCharacter>> _toRemove;

	for (TObjectPtr<APlayerCharacter> _player : playerInZone)
	{
		if (!_player)
		{
			_toRemove.Add(_player);
			continue;
		}
		_player->AddLife(-lifeToDrain);
	}

	const int _count = _toRemove.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		playerInZone.Remove(_toRemove[_i]);
	}
}

