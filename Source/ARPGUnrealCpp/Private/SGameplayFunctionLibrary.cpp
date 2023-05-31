// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"

#include "SAttributeComponent.h"

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComp) //check null
	{
		//apply damage
		return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	}
	return false;
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount,
	const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		//component that was hit
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		//if hit and simulating physics on a specific bone
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			
			// Direction = Target - Origin
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			//add impact force at impact point
			HitComp->AddImpulseAtLocation(Direction * 300000.f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}
