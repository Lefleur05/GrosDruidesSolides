// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/SpellProjectil.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/PlayerCharacter.h"
#include "Utility.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "NiagaraFunctionLibrary.h"
//#include "NiagaraSystem.h"

// Sets default values
ASpellProjectil::ASpellProjectil()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	collision = CreateDefaultSubobject<USphereComponent>("collision");
	RootComponent = collision;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("mesh");
	movement = CreateDefaultSubobject<UProjectileMovementComponent>("movement");

	mesh->SetupAttachment(RootComponent);

	bReplicates = true;
	SetReplicatingMovement(true);
	movement->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void ASpellProjectil::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SpawnLocation = GetActorLocation();
		collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (HasAuthority())
		SpawnCollide();

	

}

// Called every frame
void ASpellProjectil::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		if (FVector::Distance(GetActorLocation(), SpawnLocation) >= range && !kill)
		{
			if (explosionSize > 0.0f)
				Explosion();
			Server_SetLifeSpan(DeltaTime);
		}
	}
}

void ASpellProjectil::NotifyActorBeginOverlap(AActor* OtherActor)
{
	//LOG("OWERLAP");
	Super::NotifyActorBeginOverlap(OtherActor);
	if (HasAuthority())
		Collide(OtherActor);
}

void ASpellProjectil::SpawnCollide()
{
	if (!HasAuthority())return;
	TArray<AActor*> _overlappingActors;
	GetOverlappingActors(_overlappingActors, APlayerCharacter::StaticClass());
	const int& _count = _overlappingActors.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		//LOG_CUSTOM("SPAWN COLLIDE", Silver);
		Collide(_overlappingActors[_i]);
	}
}

void ASpellProjectil::Collide(TObjectPtr<AActor> _actor)
{
	//LOG_CUSTOM("COLLIDE", Silver);
	if (!HasAuthority())return;
	if (!IsValid(ownerCaster))return;
	if (hasCollide)return;
	if (_actor == ownerCaster) return;
	TObjectPtr<ASpellProjectil> _projectil = Cast<ASpellProjectil>(_actor);
	if (_projectil)
	{
		if (_projectil->GetOwnerCaster() == ownerCaster)return;
		if (_projectil->GetOwnerCaster() == nullptr)return;
		if (CancelSpell)
		{
			//LOG_CUSTOM("hasCollide _projectil", Silver);
			hasCollide = true;
			_projectil->Server_SetLifeSpan(DELTATIME);
			return;
		}
	}

	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(_actor);
	if (!_player || _player->GetIsDead())return;
	//LOG_CUSTOM("hasCollide _player", Silver);
	hasCollide = true;

	movement->StopMovementImmediately();

	Server_SetLifeSpan(DELTATIME);

	_player->AddLife(-damage);
	//LOG_SERVER_CLIENT(" Damage Dealt => " + FString::SanitizeFloat(-damage));
	if (explosionSize > 0.0f)
		Explosion();
	else if (knockback > 0.0f)
		Knockback(_player);
}

void ASpellProjectil::Explosion()
{
	//LOG_CUSTOM("Explosion",Black);
	if (hasExploded)return;
	if (!HasAuthority())return;
	Multi_Explosion(explosionSize);

	TArray<FHitResult> _results;
	bool _hit = UKismetSystemLibrary::SphereTraceMultiForObjects(WORLD, GetActorLocation(), GetActorLocation(), explosionSize, playerLayer, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, _results, true, FLinearColor::Red, FLinearColor::Green,2.0f);
	hasExploded = true;
	if (!_hit)return;

	TSet<TObjectPtr<AActor>> _uniqueActor;
	for (FHitResult& _result : _results)
	{
		TObjectPtr<AActor> _actorDetected = _result.GetActor();
		if (_uniqueActor.Contains(_actorDetected))continue;
		_uniqueActor.Add(_actorDetected);

		TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(_actorDetected);
		if (!_player)continue;

		_player->AddLife(-explosionDamage);
		//LOG_SERVER_CLIENT(" Damage Dealt With Explosion => " + FString::SanitizeFloat(-explosionDamage));
		if (knockback > 0.0f)
		{
			//LOG_SERVER_CLIENT(_player.GetFName().ToString());
			Knockback(_player, true);
		}

	}
}

void ASpellProjectil::Multi_Explosion_Implementation(const float& _explosionSize)
{
	//ExplosionDebug
	TArray<FHitResult> _results;
	bool _hit = UKismetSystemLibrary::SphereTraceMultiForObjects(this, GetActorLocation(), GetActorLocation(), _explosionSize, playerLayer, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, _results, true, FLinearColor::Red, FLinearColor::Green, 2.0f);

	//TODO FX
	//if (!explosionFX)return;
	//UNiagaraFunctionLibrary::Spawn

}

void ASpellProjectil::Knockback(TObjectPtr<APlayerCharacter> _player, bool _isExplosionKnockback)
{
	if (!HasAuthority() || !IsValid(_player))return;
	if (hasKnockbacked)return;
	FVector _directionNormalize;
	if (_isExplosionKnockback)
	{
		_directionNormalize = _player->GetActorLocation() - GetActorLocation();
		_directionNormalize = FVector(_directionNormalize.X, _directionNormalize.Y, 0.0f);
		_directionNormalize.Normalize();
	}
	else
	{
		_directionNormalize = GetActorForwardVector();
		_directionNormalize = FVector(_directionNormalize.X, _directionNormalize.Y, 0.0f);
		_directionNormalize.Normalize();
	}
	

	float _knockback = knockback;
	if (explosionSize > 0.0f)
	{
		float _normalizePower = 1-FVector::Distance(GetActorLocation(), _player->GetActorLocation()) / explosionSize;
		_normalizePower = _normalizePower < 0.0f ? 0.0f: (_normalizePower > 1.0f ? 1.0f: _normalizePower);
		_normalizePower = _normalizePower < (knockbackMinPowerPercent / 100.0f) ? (knockbackMinPowerPercent / 100.0f) : _normalizePower;

		_knockback = knockback * _normalizePower;
	}
	FVector _launch = _directionNormalize * FMath::Sqrt(_knockback* _player->GetCharacterMovement()->BrakingDecelerationWalking * 100.0f *2.0f);
	_launch.Z += _player->GetCharacterMovement()->JumpZVelocity;


	//hasKnockbacked = true;
	_player->LaunchCharacter(_launch, true, true);
}

void ASpellProjectil::SetStatWithItem(FItem _item)
{
	damage = _item.damage;
	
	movement->InitialSpeed = _item.projectilSpeed;
	//movement->Velocity = GetActorForwardVector() * movement->InitialSpeed;
	movement->MaxSpeed = _item.projectilSpeed;
	//movement->InitialSpeed += _item.projectilSpeed;
	SetActorScale3D(_item.size);
	range = _item.range;
	knockback = _item.knockback;
	explosionSize = _item.explosionSize;
	explosionDamage = _item.explosionDamage;
	CancelSpell = _item.CancelSpell;
}

void ASpellProjectil::Server_SetLifeSpan_Implementation(const float& _time)
{
	kill = true;
	SetLifeSpan(_time);
	Multi_SetLifeSpan(_time);
}

void ASpellProjectil::Multi_SetLifeSpan_Implementation(const float& _time)
{
	kill = true;
}

