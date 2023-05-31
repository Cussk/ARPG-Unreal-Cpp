// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

USAttributeComponent::USAttributeComponent()
{
	Health = 100;
	HealthMax = 100;
	Health = HealthMax;
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
	if(!GetOwner()->CanBeDamaged())
	{
		return false;
	}

	float OldHealth = Health;

	//clamps health between 0 and max health
	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta); //triggers event

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

