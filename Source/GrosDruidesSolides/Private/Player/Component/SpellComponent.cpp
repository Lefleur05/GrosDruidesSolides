#include "Player/Component/SpellComponent.h"
#include "GPE/SpellProjectil.h"
#include "Player/PlayerCharacter.h"
#include "Utility.h"
#include "Net/UnrealNetwork.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/ProjectileMovementComponent.h"

USpellComponent::USpellComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpellComponent::BeginPlay()
{
	Super::BeginPlay();
	player = Cast<APlayerCharacter>(GetOwner());
	if (!player)
	{
		LOG_ERROR("No Player Owner");
	}
}

void USpellComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USpellComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpellComponent, canShoot);
	DOREPLIFETIME(USpellComponent, inShoot);
	DOREPLIFETIME(USpellComponent, itemStatsPrimarySpell);
	DOREPLIFETIME(USpellComponent, itemStatsSecondarySpell);
	DOREPLIFETIME(USpellComponent, itemStatsShieldSpell);
	DOREPLIFETIME(USpellComponent, itemStatsUltimateSpell);
}

void USpellComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CLEAR_TIMER(castTimer);
	CLEAR_TIMER(shootBurst);

}

void USpellComponent::Server_SpawnSpell_Implementation(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster)
{
	if (!_spellToSpawn)return;
	if (!_ownerCaster) return;
	if (_spellStats.coneSize <= 0.0f && _spellStats.projectilNumber > 1.0f)
	{
		countBurstProjectil = _spellStats.projectilNumber;
		TIMER_LOOP_PARAM(WORLD, shootBurst, SpawnInServer(_spellToSpawn, _spellStats, _transform, _ownerCaster), 0.0f, 0.05f);

	}
	else
	{
		TArray<float> _randYaw = GetRandomAngleInCone(_spellStats.projectilNumber, _spellStats.coneSize);
		SpawnInServer(_spellToSpawn, _spellStats, _transform, _ownerCaster, _randYaw);
	}
	//Multi_SpawnSpell(_spellToSpawn, _spellStats, _transform, _ownerCaster, _randYaw);

	if (!(_spellStats.coneSize <= 0.0f && _spellStats.projectilNumber > 1.0f))
	{
		if(player)
			player->SetCanMove(true);
		inShoot = false;
	}
}

void USpellComponent::Multi_SpawnSpell_Implementation(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster, const TArray<float>& _randYaw)
{
	//TODO FX
}

void USpellComponent::Server_RequestSpawnSpell_Implementation(ESpellType _spellType)
{
	if (!canShoot || inShoot || !player)return;

	TSubclassOf<ASpellProjectil> _spellToSpawn;
	FItem _spellStats;
	FTransform _transform = player->GetSpawnProjectilPoint()->GetComponentToWorld();
	APlayerCharacter* _ownerCaster = player;
	float* _lastTimeShootSpell = nullptr;

	switch (_spellType)
	{
	case ESpellType::ST_MINOR_PROJECTILE:
		_spellToSpawn = primarySpellProjectil;
		_spellStats = itemStatsPrimarySpell;
		_lastTimeShootSpell = &lastTimeShootPrimarySpell;

		break;
	case ESpellType::ST_SECONDARY_PROJECTILE:
		_spellToSpawn = secondarySpellProjectil;
		_spellStats = itemStatsSecondarySpell;
		_lastTimeShootSpell = &lastTimeShootSecondarySpell;
		break;
	case ESpellType::ST_SHIELD_PROJECTILE:
		_spellToSpawn = shieldSpellProjectil;
		_spellStats = itemStatsShieldSpell;
		_lastTimeShootSpell = &lastTimeShootShieldSpell;
		break;
	case ESpellType::ST_ULTIMATE_PROJECTILE:
		_spellToSpawn = ultimateSpellProjectil;
		_spellStats = itemStatsUltimateSpell;
		_lastTimeShootSpell = &lastTimeShootUltimateSpell;

		break;
	default:
		return;
	}
	if (!_spellToSpawn || !_lastTimeShootSpell)return;
	if ((WORLD->GetTimeSeconds() - *_lastTimeShootSpell) < _spellStats.cooldown)return;
	inShoot = true;
	player->SetCanMove(false);
	if (_spellStats.castTime == 0.0f)
	{
		ShootSpell(_spellToSpawn, *_lastTimeShootSpell, _spellStats);
		return;
	}
	TIMER_PARAM(WORLD, castTimer, ShootSpell(_spellToSpawn, *_lastTimeShootSpell, _spellStats), _spellStats.castTime);

}

void USpellComponent::SpawnInServer(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster, const TArray<float>& _randYaw)
{
	if (!_ownerCaster) return;
	if (_spellStats.projectilNumber == 1)
		SpawnSpell(_spellToSpawn, _spellStats, _transform, _ownerCaster);
	else if (_spellStats.coneSize <= 0.0f && _spellStats.projectilNumber > 1)
		SpawnSpellInBurst(_spellToSpawn, _spellStats, _transform, _ownerCaster);
	else if (_spellStats.coneSize > 0.0f && _spellStats.projectilNumber > 1)
		SpawnSpellInCone(_spellToSpawn, _spellStats, _transform, _ownerCaster, _randYaw);
}

void USpellComponent::SpawnSpell(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster)
{
	TObjectPtr<ASpellProjectil> _projectilSpawn = WORLD->SpawnActorDeferred<ASpellProjectil>(_spellToSpawn, _transform);
	if (!_projectilSpawn) return;
	_projectilSpawn->SetOwnerCaster(_ownerCaster);
	_projectilSpawn->SetStatWithItem(_spellStats);
	UGameplayStatics::FinishSpawningActor(_projectilSpawn, _transform);
	//_projectilSpawn->SetStatWithItem(_spellStats);
}

void USpellComponent::SpawnSpellInCone(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster, const TArray<float>& _randYaw)
{

	for (int _i = 0; _i < _spellStats.projectilNumber; _i++)
	{
		FRotator _rotator = _transform.Rotator();
		
		//float _rand = FMath::FRandRange(_spellStats.coneSize * (-1), _spellStats.coneSize);
		float _rand = 0.0f;
		if (_i < _randYaw.Num())
			_rand = _randYaw[_i];
		float _yaw = _rotator.Yaw + _rand;
		//float _yaw = _rotator.Yaw + _i * 50.0f;
		_rotator = FRotator(_rotator.Pitch, _yaw, _rotator.Roll);

		//LOG("SpawnInCone => " + FString::SanitizeFloat(_rand));

		FTransform _newTransform = FTransform(_rotator,_transform.GetLocation(), _transform.GetScale3D());
		SpawnSpell(_spellToSpawn, _spellStats, _newTransform, _ownerCaster);

		/*TObjectPtr<ASpellProjectil> _projectilSpawn = WORLD->SpawnActorDeferred<ASpellProjectil>(_spellToSpawn, _newTransform);
		_projectilSpawn->SetOwnerCaster(_ownerCaster);
		_projectilSpawn->SetStatWithItem(_spellStats);
		UGameplayStatics::FinishSpawningActor(_projectilSpawn, _newTransform);
		_projectilSpawn->SetStatWithItem(_spellStats);*/
	}
}

void USpellComponent::SpawnSpellInBurst(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster)
{
	SpawnSpell(_spellToSpawn, _spellStats, _transform, _ownerCaster);
	countBurstProjectil--;
	if (countBurstProjectil <= 0)
	{
		CLEAR_TIMER(shootBurst);
		if(player)
			player->SetCanMove(true);
		inShoot = false;
	}
	
}

TArray<float> USpellComponent::GetRandomAngleInCone(int _countAngle, float _coneAngle)
{
	TArray<float> _randYaw = TArray<float>();
	for (int _i = 0; _i < _countAngle; _i++)
	{
		float _rand = FMath::FRandRange(_coneAngle * (-1), _coneAngle);
		_randYaw.Add(_rand);
	}
	return _randYaw;
}

void USpellComponent::UpdateTriItem()
{
	if (!player)return;
	itemStatsPrimarySpell = initialItemStatsPrimarySpell;
	itemStatsSecondarySpell = initialItemStatsSecondarySpell;
	itemStatsShieldSpell = initialItemStatsShieldSpell;
	itemStatsUltimateSpell = initialItemStatsUltimateSpell;
	//LOG("UpdateTriItem");

	TArray<FItem> _currentItems = player->GetCurrentItems();

	const int& _count = _currentItems.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		switch (_currentItems[_i].spell)
		{
		case ESpellType::ST_MINOR_PROJECTILE:
			itemStatsPrimarySpell += _currentItems[_i];
			break;
		case ESpellType::ST_SECONDARY_PROJECTILE:
			itemStatsSecondarySpell += _currentItems[_i];
			break;
		case ESpellType::ST_SHIELD_PROJECTILE:
			itemStatsShieldSpell += _currentItems[_i];
			break;
		case ESpellType::ST_ULTIMATE_PROJECTILE:
			itemStatsUltimateSpell += _currentItems[_i];
			break;
		default:
			break;
		}
	}
}

void USpellComponent::ShootSpell(TSubclassOf<ASpellProjectil> _spellProjectil, float& _lastTimeShootSpell, FItem _itemStatsSpell)
{
	if (!player)return;
	if (player->GetIsDead() || !_spellProjectil)return;
	if ((WORLD->GetTimeSeconds() - _lastTimeShootSpell) < _itemStatsSpell.cooldown)return;
	_lastTimeShootSpell = WORLD->GetTimeSeconds();

	Server_SpawnSpell(_spellProjectil, _itemStatsSpell, player->GetSpawnProjectilPoint()->GetComponentToWorld(), player);

	//if (GetOwner()->HasAuthority())
	//{
	//	if (_itemStatsSpell.coneSize <= 0.0f && _itemStatsSpell.projectilNumber > 1.0f)
	//	{
	//		countBurstProjectil = _itemStatsSpell.projectilNumber;
	//		//LOG_CUSTOM("ServerShoot Burst", Black);
	//		TIMER_LOOP_PARAM(WORLD, shootBurst, Multi_SpawnSpell(_spellProjectil, _itemStatsSpell, player->GetSpawnProjectilPoint()->GetComponentToWorld(), player) , 0.0f, 0.05f);
	//	}
	//	else
	//	{
	//		//LOG_CUSTOM("ServerShoot", Black);
	//		TArray<float> _randYaw = GetRandomAngleInCone(_itemStatsSpell.projectilNumber, _itemStatsSpell.coneSize);
	//		Multi_SpawnSpell(_spellProjectil, _itemStatsSpell, player->GetSpawnProjectilPoint()->GetComponentToWorld(), player, _randYaw);
	//	}
	//}
	//else
	//	

}

void USpellComponent::ShootPrimarySpell()
{
	//if (!canShoot|| inShoot)return;
	Server_RequestSpawnSpell(ESpellType::ST_MINOR_PROJECTILE);
	//if (inShoot || !player ||!primarySpellProjectil)return;
	//if ((WORLD->GetTimeSeconds() - lastTimeShootPrimarySpell) < itemStatsPrimarySpell.cooldown)return;
	////inShoot = true;
	////player->SetCanMove(false);
	//if (itemStatsPrimarySpell.castTime == 0.0f)
	//{
	//	ShootSpell(primarySpellProjectil, lastTimeShootPrimarySpell, itemStatsPrimarySpell);
	//	return;
	//}
	//TIMER(WORLD, castTimer, ShootSpell(primarySpellProjectil, lastTimeShootPrimarySpell, itemStatsPrimarySpell), itemStatsPrimarySpell.castTime);
}

void USpellComponent::ShootSecondarySpell()
{
	//if (!canShoot || inShoot)return;
	Server_RequestSpawnSpell(ESpellType::ST_SECONDARY_PROJECTILE);
}

void USpellComponent::ShootShieldSpell()
{
	//if (!canShoot || inShoot)return;
	Server_RequestSpawnSpell(ESpellType::ST_SHIELD_PROJECTILE);
}

void USpellComponent::ShootUltimatedSpell()
{
	//if (!canShoot || inShoot)return;
	Server_RequestSpawnSpell(ESpellType::ST_ULTIMATE_PROJECTILE);
}

void USpellComponent::Respawn()
{
	Server_Respawn();

}

void USpellComponent::Server_Respawn_Implementation()
{
	UpdateTriItem();
	inShoot = false;
	countBurstProjectil = 0;
	CLEAR_TIMER(castTimer);
	CLEAR_TIMER(shootBurst);

	lastTimeShootPrimarySpell = 0.0f;
	lastTimeShootSecondarySpell = 0.0f;
	lastTimeShootShieldSpell = 0.0f;
	lastTimeShootUltimateSpell = 0.0f;

	Multi_Respawn();
}

void USpellComponent::Multi_Respawn_Implementation()
{
	inShoot = false;
	countBurstProjectil = 0;
	CLEAR_TIMER(castTimer);
	CLEAR_TIMER(shootBurst);
}
