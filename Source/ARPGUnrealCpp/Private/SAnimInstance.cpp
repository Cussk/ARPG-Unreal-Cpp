// Fill out your copyright notice in the Description page of Project Settings.


#include "SAnimInstance.h"

#include "GameplayTagContainer.h"
#include "SActionComponent.h"

void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//get action component from owning actor
	AActor* OwningActor = GetOwningActor();
	if (OwningActor)
	{
		ActionComp = Cast<USActionComponent>(OwningActor->GetComponentByClass(USActionComponent::StaticClass()));
	}
}



void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");
	if (ActionComp) //check null
	{
		//set bIsStunned true or false based on if tag exists
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
	}
}