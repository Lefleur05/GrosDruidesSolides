// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/CombatDummy.h"
#include <Components/CapsuleComponent.h>
#include <Net/UnrealNetwork.h>

ACombatDummy::ACombatDummy()
{
	PrimaryActorTick.bCanEverTick = true;

	collision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	RootComponent = collision;
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	mesh->SetupAttachment(RootComponent);

}

void ACombatDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACombatDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACombatDummy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACombatDummy, currentLife);

}

void ACombatDummy::AddLife(const float& _lifeToAdd)
{
	if (!HasAuthority()) return;
	currentLife += _lifeToAdd;
	if (currentLife > maxLife)
		currentLife = maxLife;
	else if (currentLife <= 0.0f)
	{
		currentLife = maxLife;
	}
	onDummyAddLife.Broadcast(this, _lifeToAdd);
}

