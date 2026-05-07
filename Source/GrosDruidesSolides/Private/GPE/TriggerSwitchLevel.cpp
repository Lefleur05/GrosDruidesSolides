

#include "GPE/TriggerSwitchLevel.h"
#include "TriggerSwitchLevel.h"
#include <Components/BoxComponent.h>
#include <Player/PlayerCharacter.h>
#include "Utility.h"
#include "GameFramework/GameStateBase.h"
#include <GameMode/LobbyGameModeBase.h>

ATriggerSwitchLevel::ATriggerSwitchLevel()
{
	PrimaryActorTick.bCanEverTick = true;
	collision = CreateDefaultSubobject<UBoxComponent>("Collision");
	RootComponent = collision;
}

void ATriggerSwitchLevel::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATriggerSwitchLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriggerSwitchLevel::NotifyActorBeginOverlap(AActor* _otherActor)
{
	//TODO What happens if someone disconnects in the actor?
	Super::NotifyActorBeginOverlap(_otherActor);
	if (!HasAuthority())return;

	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(_otherActor);
	if (_player && !listPlayerContain.Contains(_player))
		listPlayerContain.Add(_player);

	const int _count = listPlayerContain.Num();
	for (int _i = 0; _i < _count; _i++)
	{
		if (!listPlayerContain[_i])
			listPlayerContain.RemoveAt(_i);
	}

	if (WORLD->GetGameState()->PlayerArray.Num() == listPlayerContain.Num())
	{
		if (!GetWorldTimerManager().IsTimerActive(timerStartGame))
		{
			TIMER(this, timerStartGame, StartGame(), timeStartGame);
			LOG("Start Timer for start game");
		}
	}

}

void ATriggerSwitchLevel::NotifyActorEndOverlap(AActor* _otherActor)
{
	Super::NotifyActorEndOverlap(_otherActor);
	if (!HasAuthority())return;

	TObjectPtr<APlayerCharacter> _player = Cast<APlayerCharacter>(_otherActor);
	if (_player && listPlayerContain.Contains(_player))
		listPlayerContain.Remove(_player);

	if (WORLD->GetGameState()->PlayerArray.Num() != listPlayerContain.Num())
	{
		if (GetWorldTimerManager().IsTimerActive(timerStartGame))
			CLEAR_TIMER(timerStartGame); 
	}
}

void ATriggerSwitchLevel::StartGame()
{
	if (!HasAuthority())return;
	if (clearIteam)
	{
		ClearIteam();
	}
	LOG("Server Travel to => " + targetLevel);
	// For server travel to a map and choose the game mode

	WORLD->ServerTravel("/Game/Level/"+targetLevel + "?game=/Game/Blueprint/GameMode/BP_CarrouselGameModeBase.BP_CarrouselGameModeBase_C");	
}

void ATriggerSwitchLevel::ClearIteam()
{
	if(GetWorld())
		if(TObjectPtr<ALobbyGameModeBase> _gameMode = Cast<ALobbyGameModeBase>(GetWorld()->GetAuthGameMode()))
			_gameMode->ClearItem();
}

