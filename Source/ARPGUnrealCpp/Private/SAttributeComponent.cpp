// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

USAttributeComponent::USAttributeComponent()
{
	Health = 100;
	HealthMax = 100;
	Health = HealthMax;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}


float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	float OldHealth = Health;

	//clamps health between 0 and max health
	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta); //triggers event

	return ActualDelta != 0;
}

//gets attribute component of designated actor
USAttributeComponent* USAttributeComponent::GetAttrutes(AActor* FromActor)
{
	if (FromActor) //check nullptr
	{
		return FromActor->FindComponentByClass<USAttributeComponent>();
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* FromActor)
{
	USAttributeComponent* AttributeComponent = GetAttrutes(FromActor);
	if (AttributeComponent) //check nullptr
	{
		return AttributeComponent->IsAlive();
	}

	return false; //returns dead if no attribute component
}

