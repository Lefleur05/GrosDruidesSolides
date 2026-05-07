// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/Blades.h"
#include <GameFramework/RotatingMovementComponent.h>
#include <Player/PlayerCharacter.h>
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABlades::ABlades()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	bladeOne = CreateDefaultSubobject<UStaticMeshComponent>("bladeOne");
	bladeTwo = CreateDefaultSubobject<UStaticMeshComponent>("bladeTwo");
	bladeTree = CreateDefaultSubobject<UStaticMeshComponent>("bladeTree");
	bladeFour = CreateDefaultSubobject<UStaticMeshComponent>("bladeFour");
	movement = CreateDefaultSubobject<URotatingMovementComponent>("movement");
	capsuleCollision = CreateDefaultSubobject<UCapsuleComponent>("capsuleCollision");

	bladeOne->SetupAttachment(RootComponent);
	bladeTwo->SetupAttachment(RootComponent);
	bladeTree->SetupAttachment(RootComponent);
	bladeFour->SetupAttachment(RootComponent);
	AddOwnedComponent(movement);
	capsuleCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABlades::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())return;
	capsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ABlades::OnOverlapBegin);
	capsuleCollision->OnComponentEndOverlap.AddDynamic(this, &ABlades::OnOverlapEnd);

	TIMER_LOOP(this, damageTimer, &ABlades::Damage, timeDamageLoop, timeDamageLoop);
}

// Called every frame
void ABlades::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlades::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CLEAR_TIMER(damageTimer);
}

void ABlades::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())return;
	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(OtherActor);
	if (_player)
	{
		players.Add(_player);
		_player->AddLife(-damagePerBlade);

	}

}

void ABlades::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())return;
	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(OtherActor);
	if (_player && players.Contains(_player))
	{
		players.Remove(_player);

	}
}

void ABlades::Damage()
{
	for (TObjectPtr<APlayerCharacter> _player : players)
	{
		if (_player)
		{
			_player->AddLife(-damagePerBlade);
		}
	}
}

