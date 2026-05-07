// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/MultyplayerCamera.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "Utility.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode/MultyGameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include <Net/UnrealNetwork.h>
#include <Physics/NetworkPhysicsComponent.h>

// Sets default values
AMultyplayerCamera::AMultyplayerCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	springArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	camera = CreateDefaultSubobject<UCameraComponent>("Camera");

	springArm->SetupAttachment(RootComponent);
	camera->SetupAttachment(springArm);

}

// Called when the game starts or when spawned
void AMultyplayerCamera::BeginPlay()
{
	Super::BeginPlay();
	if(setHasViewTarget)
		WORLD->GetFirstPlayerController()->SetViewTarget(this);
	
}

// Called every frame
void AMultyplayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (setHasViewTarget)
		WORLD->GetFirstPlayerController()->SetViewTarget(this);
	if (HasAuthority())
	{
		//FRAME_LOG("Count Player for camera => "+ FString::SanitizeFloat(playerList.Num()));
		if (WORLD->GetGameState() && WORLD->GetGameState()->PlayerArray.Num() != playerList.Num())
		{
			playerList.Empty();
			const int _count = WORLD->GetGameState()->PlayerArray.Num();
			for (int _i = 0; _i < _count; _i++)
			{
				if(WORLD->GetGameState()->PlayerArray[_i])
					if(TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(WORLD->GetGameState()->PlayerArray[_i]->GetPawn()))
						playerList.Add(_player);
			}
		}
	}

	APlayerController* _pc = GetWorld()->GetFirstPlayerController();
	if (_pc && _pc->IsLocalController())
	{
		
		CameraLocationWithAverage();
		CameraZoom();
	}
	
}

void AMultyplayerCamera::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMultyplayerCamera, playerList);
}

void AMultyplayerCamera::CameraLocationWithAverage()
{
	FVector _averagePlayersLoc;
	int _countPlayerAlive = 0;
	const int _count = playerList.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (playerList[_i] && !playerList[_i]->GetIsDead())
		{
			_averagePlayersLoc += playerList[_i]->GetActorLocation();
			_countPlayerAlive++;
		}
	}
	if (_countPlayerAlive == 0)return;
	_averagePlayersLoc /= _countPlayerAlive;
	float _moveSpeed = moveSpeed;
	if (_countPlayerAlive == 1)
		_moveSpeed = moveSpeedOnePlayer;
	//FRAME_LOG("Count Player Alive => " + FString::SanitizeFloat(_countPlayerAlive));

	FVector _newLoc = FMath::VInterpConstantTo(GetActorLocation(), _averagePlayersLoc, DELTATIME, _moveSpeed);
	SetActorLocation(_newLoc);
	//SetActorLocation(_averagePlayersLoc);
}

void AMultyplayerCamera::CameraZoom()
{
	float _greatestDistanceBetweenTwoPlayers = 0.0f;
	const int _count = playerList.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (playerList[_i] && !playerList[_i]->GetIsDead())
		{
			for (int _y = 0; _y < _count; _y++)
			{
				if (playerList[_y] && !playerList[_y]->GetIsDead())
				{
					float _distance = FVector::Distance(playerList[_i]->GetActorLocation(), playerList[_y]->GetActorLocation());
					if (_distance > _greatestDistanceBetweenTwoPlayers)
						_greatestDistanceBetweenTwoPlayers = _distance;
				}
			}
		}
	}
	
	float _newZoom = FMath::GetMappedRangeValueClamped(rangeDistance, rangeZoom, _greatestDistanceBetweenTwoPlayers);
	_newZoom = FMath::FInterpConstantTo<float>(springArm->TargetArmLength, _newZoom * minZoom, DELTATIME, zoomSpeed);
	springArm->TargetArmLength = _newZoom;
}
