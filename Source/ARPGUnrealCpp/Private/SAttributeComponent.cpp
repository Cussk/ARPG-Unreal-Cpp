// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

//console sommand multiply damage by float
static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("s.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);

USAttributeComponent::USAttributeComponent()
{
	Health = 100;
	HealthMax = 100;
	Health = HealthMax;

	//sets class to replicate for multiplayer
	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	//reduce health to 0 (Kill)
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}

float USAttributeComponent::GetHealth()
{
	return Health;
}


float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	//does not apply damage if owner can be damaged is false
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	//if doing damage apply damage multiplier from console
	if (Delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	float OldHealth = Health;

	//clamps health between 0 and max health
	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - OldHealth;
	//OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta); //triggers event

	//only call event if health actually changes
	if (ActualDelta != 0.0f)
	{
		//triggers OnHealthChanged event if on server replicates to all clients, if called on client only triggers on client
		MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
	}

	//Died
	if (ActualDelta < 0.0f && Health == 0.0f)
	{
		ASGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (GameMode) // check null
		{
			GameMode->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return ActualDelta != 0;
}

//gets attribute component of designated actor
USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor) //check nullptr
	{
		return FromActor->FindComponentByClass<USAttributeComponent>();
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* FromActor)
{
	USAttributeComponent* AttributeComponent = GetAttributes(FromActor);
	if (AttributeComponent) //check nullptr
	{
		return AttributeComponent->IsAlive();
	}

	return false; //returns dead if no attribute component
}

//Triggers OnHealthChanged event for multiplayer
void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//whenever condition changes on server send to all clients
	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, HealthMax);

	//DOREPLIFETIME_CONDITION(USAttributeComponent, HealthMax, COND_InitialOnly);
}
