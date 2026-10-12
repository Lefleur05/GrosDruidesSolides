// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/PlatformMover.h"
#include "Utility.h"
#include <Net/UnrealNetwork.h>

// Sets default values
APlatformMover::APlatformMover()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlatformMover::BeginPlay()
{
	Super::BeginPlay();
	startTime = WORLD->GetTimeSeconds();
}

// Called every frame
void APlatformMover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MovePlateform();
	if(HasAuthority())
		currentTime = WORLD->GetTimeSeconds() - startTime;
	FRAME_LOG("Current Time = " + FString::SanitizeFloat(currentTime));

}

void APlatformMover::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlatformMover, startTime);
	DOREPLIFETIME(APlatformMover, currentTime);
}

void APlatformMover::MovePlateform()
{
	for(FPlateformToMoveInfo _plateform : allPlateformToMove)
	{
		if (!_plateform.plateform)continue;
		if (currentTime < _plateform.startMovingTime)continue;
		float _alpha = (currentTime - _plateform.startMovingTime) / (_plateform.endMovingTime - _plateform.startMovingTime);
		if (_alpha < 0.0f || _alpha > 1.0f)continue;
		//FRAME_LOG("Alpha = " + FString::SanitizeFloat(_alpha));
		FVector _newLoc = UKismetMathLibrary::VLerp(_plateform.startLocation, _plateform.endLocation, _alpha);
		_plateform.plateform->SetActorLocation(_newLoc);
	}
}

