// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include <EnhancedInputSubsystems.h>
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include <EnhancedInputLibrary.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Utility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GPE/SpellProjectil.h"
//#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include <Net/UnrealNetwork.h>
//#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/ChildActorComponent.h"
#include "Player/Component/SpellComponent.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/MultyplayerCamera.h"
#include <GameMode/PlayerState/InfoPlayerState.h>
#include <GameMode/LobbyGameModeBase.h>
#include "Player/Component/CameraCustomComponent.h"

#include "GameAnalytics.h"
#include "GameAnalyticsModule.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	forwardArrow = CreateDefaultSubobject<UArrowComponent>("ForwardArrow");
	spawnProjectilPoint = CreateDefaultSubobject<UChildActorComponent>("SpawnProjectil");
	spell = CreateDefaultSubobject<USpellComponent>("Spell");
	springArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	cameraCustom = CreateDefaultSubobject<UCameraCustomComponent>("CameraCustom");

	forwardArrow->SetupAttachment(RootComponent);
	spawnProjectilPoint->SetupAttachment(RootComponent);
	springArm->SetupAttachment(RootComponent);
	camera->SetupAttachment(springArm);

	AddOwnedComponent(spell);
	AddOwnedComponent(cameraCustom);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//GetCharacterMovement()->GravityScale = 1.0f;
	//GetCharacterMovement()->MaxAcceleration = 1500.0f;
	//GetCharacterMovement()->MaxWalkSpeed = 6000.0f;
	//GetCharacterMovement()->MaxStepHeight = 45.0f;
	//GetCharacterMovement()->SetWalkableFloorAngle(45.0f);
	//GetCharacterMovement()->AirControl = 0.35f;
	
	if (IsLocallyControlled())
	{
		GetMesh()->SetMaterial(0, clientMaterial);
		GetMesh()->SetMaterial(1, clientMaterial2);

		if (TObjectPtr<APlayerController> _playerController = Cast<APlayerController>(GetController()))
		{
			_playerController->SetViewTargetWithBlend(this, 0.0f);
			//LOG(FString("ViewTarget: ") + (_playerController->GetViewTarget() ? _playerController->GetViewTarget()->GetName() : "NULL"));
		}
	}
	if (springArm)
		springArm->SetAbsolute(true, true, false);

	spell->UpdateTriItem();
	UpdateStatsItem();

	if (IsLocallyControlled())
	{
		const int _count = currentItems.Num();
		for (int _i = 0; _i < _count; _i++)
		{
			if (GetPlayerState())
				LOG_CUSTOM(GetPlayerState()->GetUniqueId().ToString() + " => " + currentItems[_i].GetItemName(),Purple);
		}
	}


	UGameAnalytics* GameAnalytics = FGameAnalyticsModule::Get().GetInstance();

	// initialize the SDK with your keys
	GameAnalytics->Initialize("0cb20cc20cd456fc8a409f0e9047d064","5424a77773f94792b9567d6fd5bc580775240950");
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLocallyControlled())
	{
		FColor _color = (IsLocallyControlled() ? FColor::Magenta : FColor(255.0f, 192.0f, 203.0f));
		FRAME_LOG_CUSTOM_C(GetName() + " CurrentLife = " + FString::SanitizeFloat(currentLife), _color);
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!IsLocallyControlled())return; //If I'm the one who owns that pawn, then I'll make a move with it
	UEnhancedInputComponent* _input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!_input)return;
	_input->BindAction(moveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Movement);
	_input->BindAction(dashAction, ETriggerEvent::Started, this, &APlayerCharacter::Dash);

	if (spell)
	{
		_input->BindAction(shootPrimarySpellAction, ETriggerEvent::Started, spell.Get(), &USpellComponent::ShootPrimarySpell);
		_input->BindAction(shootSecondarySpellAction, ETriggerEvent::Started, spell.Get(), &USpellComponent::ShootSecondarySpell);
		_input->BindAction(shootShieldSpellAction, ETriggerEvent::Started, spell.Get(), &USpellComponent::ShootShieldSpell);
		_input->BindAction(shootUltimatedSpellAction, ETriggerEvent::Started, spell.Get(), &USpellComponent::ShootUltimatedSpell);
		//LOG("SetupPlayerInputComponent");
	}
	
}

void APlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
}

void APlayerCharacter::PossessedBy(AController* _controller)
{
	Super::PossessedBy(_controller);
	if (TObjectPtr<AController> _playerController = _controller)
	{
		if (TObjectPtr<AInfoPlayerState> _playerState = Cast<AInfoPlayerState>(_playerController->PlayerState))
		{
			currentItems = _playerState->GetCurrentItems();
			spell->UpdateTriItem();
			UpdateStatsItem();
		}
	}	
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, canTakeDamage);
	DOREPLIFETIME(APlayerCharacter, isDead);
	DOREPLIFETIME(APlayerCharacter, canMove);
	DOREPLIFETIME(APlayerCharacter, currentLife);
	DOREPLIFETIME(APlayerCharacter, maxLife);
	DOREPLIFETIME(APlayerCharacter, currentItems);
	DOREPLIFETIME(APlayerCharacter, cooldownDash);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CLEAR_TIMER(timerCooldownDash);

}


void APlayerCharacter::Movement(const FInputActionValue& _value)
{
	if (isDead || !canMove || !GetController() || !GetController()->GetViewTarget())return;
	const FVector2D& _input = _value.Get<FVector2D>();
	//const FRotator& _rotation = GetController()->GetViewTarget()->GetActorRotation();
	const FRotator& _rotation = camera->GetComponentRotation();
	const FVector& _rightVector = UKismetMathLibrary::GetRightVector(FRotator(0, _rotation.Yaw, _rotation.Roll));
	const FVector& _forwardVector = UKismetMathLibrary::GetForwardVector(FRotator(0, _rotation.Yaw, 0));

	const FVector& _move = (_forwardVector * _input.X + _rightVector * _input.Y);

	AddMovementInput(_move);
}

void APlayerCharacter::Dash()
{
	if (isDead)return;
	if (!HasAuthority())
	{
		canDash = false;
		TIMER(this, timerCooldownDash, DashRecover(), cooldownDash < 0.5f ? 0.5f : cooldownDash);
	}
	Server_Dash();
}

void APlayerCharacter::Server_Dash_Implementation()
{
	if (isDead || !canDash)return;

	canDash = false;
	TIMER(this, timerCooldownDash, DashRecover(), cooldownDash < 0.5f ? 0.5f : cooldownDash);
	LaunchCharacter(GetActorForwardVector() * 2000.0f + FVector(0.0f, 0.0f, 300.0f), true, true);

	Multi_Dash();
}

void APlayerCharacter::Multi_Dash_Implementation()
{
	//TODO FX Dash
}

void APlayerCharacter::DashRecover()
{
	canDash = true;
}

void APlayerCharacter::Death()
{
	if (currentLife > 0.0f || isDead)return;
	Server_Death();
	if (!HasAuthority())
	{
		isDead = true;
		canMove = false;
	}
}

void APlayerCharacter::Server_Death_Implementation()
{
	if (currentLife > 0.0f || isDead)return;

	canMove = false;
	isDead = true;
	if(GetCharacterMovement())
		GetCharacterMovement()->DisableMovement();
	if(GetCapsuleComponent())
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Multi_Death();

	onDeath.Broadcast(this);
}

void APlayerCharacter::Multi_Death_Implementation()
{
	RagDoll();
	forwardArrow->SetVisibility(false);
}

void APlayerCharacter::RagDoll()
{
	TObjectPtr<USkeletalMeshComponent> _mesh = GetMesh();
	if (!_mesh)return;
	_mesh->SetSimulatePhysics(true);
	_mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	_mesh->WakeAllRigidBodies();
	_mesh->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true));
}

void APlayerCharacter::DisableRagDoll()
{
	TObjectPtr<USkeletalMeshComponent> _mesh = GetMesh();
	if (!_mesh)return;
	_mesh->SetSimulatePhysics(false);
	_mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(RootComponent)
		_mesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	_mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -135.0f));
	_mesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
}

void APlayerCharacter::Server_Respawn_Implementation(bool _deleteItem)
{
	if (!isDead)return;
	isDead = false;
	canMove = true;
	currentLife = maxLife;

	if (_deleteItem)
		currentItems = TArray<FItem>();

	UpdateStatsItem();
	if(spell)
		spell->Respawn();
	
	Multi_Respawn(_deleteItem);

	if (GetCharacterMovement())
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (GetCapsuleComponent())
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	onRespawn.Broadcast(this);
}

void APlayerCharacter::Multi_Respawn_Implementation(bool _deleteItem)
{
	isDead = false;
	canMove = true;
	DisableRagDoll();
	forwardArrow->SetVisibility(true);

}

void APlayerCharacter::Respawn(bool _deleteItem)
{
	if (!isDead)return;

	Server_Respawn(_deleteItem);
	if (!HasAuthority())
	{
		isDead = false;
		canMove = true;
	}
}
		

void APlayerCharacter::UpdateStatsItem()
{
	if (!HasAuthority())return;
	Server_UpdateStatsItem();		
}

void APlayerCharacter::Server_UpdateStatsItem_Implementation()
{
	float _newMaxLife = initialMaxLife;
	float _newSpeed = initialMoveSpeed;
	float _newDashCooldown = initialCooldownDash;

	const int& _count = currentItems.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if(GetPlayerState())
			LOG_CUSTOM(GetPlayerState()->GetUniqueId().ToString() + " => " + currentItems[_i].GetItemName(), Orange);
		_newSpeed += currentItems[_i].speed;
		_newMaxLife += currentItems[_i].hp;
		_newDashCooldown *= currentItems[_i].dashColldown;
	}

	if(GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = _newSpeed;

	if (maxLife != 0.0f)
	{
		currentLife = currentLife / maxLife * _newMaxLife;
		maxLife = _newMaxLife;
		currentLife = currentLife >= maxLife ? maxLife : currentLife;
		cooldownDash = _newDashCooldown;
	}

	Multi_UpdateStatsItem();
}

void APlayerCharacter::Multi_UpdateStatsItem_Implementation()
{
	//TODO update UI
}

void APlayerCharacter::AddLife(const float& _lifeToAdd)
{
	if (!canTakeDamage) return;
	if (!HasAuthority()) return;
	if (isDead)return;
	currentLife += _lifeToAdd;
	if (currentLife > maxLife)
		currentLife = maxLife;
	else if (currentLife <= 0.0f)
	{
		currentLife = 0.0f;
		Death();
	}
}

void APlayerCharacter::AddItem(const FItem& _item)
{
	currentItems.Add(_item);
	UpdateStatsItem();
	spell->UpdateTriItem();
	onAddItem.Broadcast(this);
}