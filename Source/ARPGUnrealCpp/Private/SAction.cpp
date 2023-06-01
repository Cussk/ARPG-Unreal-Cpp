// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include "SActionComponent.h"


bool USAction::CanStart_Implementation(AActor* Instigator)
{
	if (IsRunning())
	{
		return false;
	}

	USActionComponent* ActionComponent = GetOwningComponent();

	if (ActionComponent->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}

	return true;
}

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	USActionComponent* ActionComponent = GetOwningComponent();
	//add tags to container
	ActionComponent->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), *GetNameSafe(this));

	ensureAlways(bIsRunning);

	USActionComponent* ActionComponent = GetOwningComponent();
	//remove tags from container
	ActionComponent->ActiveGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;
}

//implement proper GetWorld() for UObjects
UWorld* USAction::GetWorld() const
{
	// Outer is set when creating action via NewObject<T>
	UActorComponent* Component = Cast<UActorComponent>(GetOuter());
	if (Component) //check null
	{
		return Component->GetWorld();
	}

	return nullptr;
}


USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}

bool USAction::IsRunning() const
{
	return bIsRunning;
}
