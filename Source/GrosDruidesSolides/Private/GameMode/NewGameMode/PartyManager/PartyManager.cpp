
#include "GameMode/NewGameMode/PartyManager/PartyManager.h"
#include "GameMode/NewGameMode/AGDSGameMode.h"
#include "GameFramework/GameStateBase.h"
#include <Net/UnrealNetwork.h>
#include "PlayerController/MultyPlayerController.h"
#include "Player/PlayerCharacter.h"

// Sets default values
APartyManager::APartyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APartyManager::BeginPlay()
{
	Super::BeginPlay();
	

	
}

// Called every frame
void APartyManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!HasAuthority())
        return;

    if (pieAllPlayerSpawn)
        return;

    if (!gameMode || !gameMode->GameState)
        return;

    const int32 PlayerStateCount = gameMode->GameState->PlayerArray.Num();

    TArray<TObjectPtr<APlayerController>> Controllers;

    for (FConstPlayerControllerIterator It =
        GetWorld()->GetPlayerControllerIterator();
        It;
        ++It)
    {
        APlayerController* PC = It->Get();

        if (!PC)
            continue;

        if (!PC->PlayerState)
            continue;

        Controllers.Add(PC);
    }

    if (PlayerStateCount < 2)//TODO change min, need to be count of player in lobby
        return;

    if (Controllers.Num() != PlayerStateCount)
        return;
    LOG_ERROR(TEXT("PartyManager : PlayerStates = ") + FString::FromInt(PlayerStateCount) + TEXT(" | Controllers = ") + FString::FromInt(Controllers.Num())
    );
    LOG_ERROR(TEXT("AAAAAAAA ALL PLAYERS READY"));

    pieAllPlayerSpawn = true;

    allPlayers = gameMode->SpawnAllPlayers(Controllers);
}

void APartyManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(APartyManager, allPlayers);
}

void APartyManager::InitParty(const TArray<TObjectPtr<APlayerCharacter>>& _players)
{
	allPlayers = _players;
}

void APartyManager::SpawnUI()
{
    for (TObjectPtr<APlayerCharacter> _player : allPlayers)
    {
        if (!_player) continue;
        if (TObjectPtr<AMultyPlayerController> _playerController = Cast<AMultyPlayerController>(_player->GetController()))
        {
            if (uiForLevel)
            {
                _playerController->Client_InitializeHUD(uiForLevel);
            }
        }

    }
}

