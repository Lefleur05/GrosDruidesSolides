// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/CameraCustomComponent.h"
#include "Player/PlayerCharacter.h"
#include "Utility.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/GameplayStatics.h>
// Sets default values for this component's properties
UCameraCustomComponent::UCameraCustomComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCameraCustomComponent::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<APlayerCharacter>(OWNER);
	if (player && player->IsLocallyControlled())
	{
		springArm = player->GetSpringArm();
		camera = player->GetCamera();

		FString _levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		LOG("Level Name => " + _levelName);
		if (dataAsset && dataAsset->cameraParamPerMap.Contains(_levelName))
		{
			FCameraParam _param =
				param = dataAsset->cameraParamPerMap[_levelName];
		}
		else
		{
			param = FCameraParam();
		}

		if (springArm)
		{
			LOG("SpringArm Rotation => " + param.initialRotation.ToString());
			springArm->SetWorldRotation(param.initialRotation);
		}
		else if (!springArm)
		{
			LOG_ERROR("No SpringArm In Camera");
		}
	}
	else if (!player)
	{
		LOG_ERROR("No Player In Camera");
	}
}


// Called every frame
void UCameraCustomComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (player && player->IsLocallyControlled())
	{
		//LOG("MoveCamera");
		CameraLocationWithAverage();
		CameraZoom();
	}
}

void UCameraCustomComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCameraCustomComponent, playerList);
	//DOREPLIFETIME(UCameraCustomComponent, param);
}

void UCameraCustomComponent::CameraLocationWithAverage()
{
	FRAME_LOG("CameraLocationWithAverage");
	if (!springArm)return;
	FVector _averagePlayersLoc = FVector(0.0f);
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
	FRAME_LOG("Count Player Alive => " + FString::SanitizeFloat(_countPlayerAlive) + " / " + FString::FromInt(_count));
	if (_countPlayerAlive == 0)return;
	_averagePlayersLoc /= _countPlayerAlive;
	float _moveSpeed = param.moveSpeed;
	if (_countPlayerAlive == 1)
		_moveSpeed = param.moveSpeedOnePlayer;

	FVector _newLoc = FMath::VInterpConstantTo(springArm->GetComponentLocation(), _averagePlayersLoc, DELTATIME, _moveSpeed);
	springArm->SetWorldLocation(_newLoc);
	FRAME_LOG(_newLoc.ToString());

	//SetActorLocation(_newLoc);
}

void UCameraCustomComponent::CameraZoom()
{
	if (!springArm)return;
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

	float _newZoom = FMath::GetMappedRangeValueClamped(param.rangeDistance, param.rangeZoom, _greatestDistanceBetweenTwoPlayers);
	_newZoom = FMath::FInterpConstantTo<float>(springArm->TargetArmLength, _newZoom * param.minZoom, DELTATIME, param.zoomSpeed);
	springArm->TargetArmLength = _newZoom;
}

