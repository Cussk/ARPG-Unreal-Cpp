// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"


ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	// Continuous timer to spawn in more bots.
	// Actual amount of bots and whether its allowed to spawn determined by spawn logic later in the chain...
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

//kill all active bots
void ASGameModeBase::KillAll()
{
	for (ASAICharacter* Bot : TActorRange<ASAICharacter>(GetWorld()))
	{
		USAttributeComponent* AIAttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AIAttributeComp) && AIAttributeComp->IsAlive())
		{
			AIAttributeComp->Kill(this); // @fixme: pass in player? for kill credit
		}
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	//iterates through the Bot actors in world and increases count of alive Bots
	int32 NumOfAliveBots = 0;
	for (ASAICharacter* Bot : TActorRange<ASAICharacter>(GetWorld()))
	{
		USAttributeComponent* AIAttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AIAttributeComp) && AIAttributeComp->IsAlive())
		{
			++NumOfAliveBots;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NumOfAliveBots);

	float MaxBotCount = 10.0f;

	//set max bot count based on difficulty curve asset
	if (DifficultyCurve) //check null
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	//caps amount of bots that can spawn
	if (NumOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return; //early out.  Don't run EQS query if already at bot capacity
	}

	//runs eqs query
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance)) //checks null
	{
		//event trigger
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
	}
}

//results of query
void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	//array of vector locations returned as results from eqs
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	//if array has an index more than 0 spawn an actor
	if (Locations.Num() > 0)
	{
		if (Locations.IsValidIndex(0))
		{
			GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

			// Track all the used spawn locations
			DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
		}
	}
}

