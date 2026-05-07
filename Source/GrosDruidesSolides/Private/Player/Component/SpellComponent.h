// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemData/FItem.h"
#include "Utility.h"
#include "SpellComponent.generated.h"

class ASpellProjectil;
class APlayerCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class USpellComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) TObjectPtr<APlayerCharacter> player = nullptr;

	UPROPERTY(EditAnywhere, Replicated) bool canShoot = true;
	UPROPERTY(EditAnywhere, Replicated) bool inShoot = false;
	FTimerHandle castTimer;

	UPROPERTY(EditAnywhere) float lastTimeShootPrimarySpell = -100.0f;
	UPROPERTY(EditAnywhere) float lastTimeShootSecondarySpell = -100.0f;
	UPROPERTY(EditAnywhere) float lastTimeShootShieldSpell = -100.0f;
	UPROPERTY(EditAnywhere) float lastTimeShootUltimateSpell = -100.0f;

	UPROPERTY(EditAnywhere) TSubclassOf<ASpellProjectil> primarySpellProjectil = nullptr;
	UPROPERTY(EditAnywhere) TSubclassOf<ASpellProjectil> secondarySpellProjectil = nullptr;
	UPROPERTY(EditAnywhere) TSubclassOf<ASpellProjectil> shieldSpellProjectil = nullptr;
	UPROPERTY(EditAnywhere) TSubclassOf<ASpellProjectil> ultimateSpellProjectil = nullptr;

	UPROPERTY(EditAnywhere) FItem initialItemStatsPrimarySpell;
	UPROPERTY(EditAnywhere) FItem initialItemStatsSecondarySpell;
	UPROPERTY(EditAnywhere) FItem initialItemStatsShieldSpell;
	UPROPERTY(EditAnywhere) FItem initialItemStatsUltimateSpell;

	FTimerHandle shootBurst;
	UPROPERTY(EditAnywhere)int countBurstProjectil = 0;

	UPROPERTY(EditAnywhere, Replicated) FItem itemStatsPrimarySpell;
	UPROPERTY(EditAnywhere, Replicated) FItem itemStatsSecondarySpell;
	UPROPERTY(EditAnywhere, Replicated) FItem itemStatsShieldSpell;
	UPROPERTY(EditAnywhere, Replicated) FItem itemStatsUltimateSpell;

public:
	FORCEINLINE void SetCanShoot(bool _canShoot) { canShoot = _canShoot; LOG("Set Can Shoot = " + (canShoot ? " true" : " false")); }

public:	
	USpellComponent();

protected:
	virtual void BeginPlay() override;	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UFUNCTION(Server, Reliable) void Server_RequestSpawnSpell(ESpellType _spellType);
	UFUNCTION(Server, Reliable) void Server_SpawnSpell(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster);
	UFUNCTION(NetMulticast, Reliable) void Multi_SpawnSpell(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster,const TArray<float>& _randYaw = TArray<float>());
	void SpawnInServer(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster, const TArray<float>& _randYaw = TArray<float>());
	void SpawnSpell(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster);
	void SpawnSpellInCone(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster, const TArray<float>& _randYaw = TArray<float>());
	void SpawnSpellInBurst(TSubclassOf<ASpellProjectil> _spellToSpawn, FItem _spellStats, FTransform _transform, APlayerCharacter* _ownerCaster);

	TArray<float> GetRandomAngleInCone(int _countAngle, float _coneAngle);

public:
	void UpdateTriItem();
	void ShootSpell(TSubclassOf<ASpellProjectil> _spellProjectil, float& _lastTimeShootSpell, FItem itemStatsSpell);
	void ShootPrimarySpell();
	void ShootSecondarySpell();
	void ShootShieldSpell();
	void ShootUltimatedSpell();

	void Respawn();
	UFUNCTION(Server, Reliable)void Server_Respawn();
	UFUNCTION(NetMulticast, Reliable) void Multi_Respawn();

};
