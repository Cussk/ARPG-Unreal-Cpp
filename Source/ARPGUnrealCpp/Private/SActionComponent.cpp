// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"

#include "SAction.h"
#include "ARPGUnrealCpp/ARPGUnrealCpp.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//sets class to replicate for multiplayer
	SetIsReplicatedByDefault(true);
}


void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	//server only
	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
}


void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);

	// Draw All Actions
	for (USAction* Action : Actions)
	{
		FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;

		FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s : IsRunning: %s : Outer: %s"),
			//name of character
			*GetNameSafe(GetOwner()),
			//action name
			*Action->ActionName.ToString(),
			//is action currently running
			Action->IsRunning() ? TEXT("true") : TEXT("false"),
			//outer name
			*GetNameSafe(Action->GetOuter()));

		LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	}
}

void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))// if not null
	{
		return;
	}

	//create new action
	USAction* NewAction = NewObject<USAction>(this, ActionClass);
	if (ensure(NewAction)) //check null
	{
		//add new action to array
		Actions.Add(NewAction);

		//if autostart is true automatically call start action
		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}

}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	//is not null and not currently running
	if (ensure(ActionToRemove && !ActionToRemove->IsRunning())) 
	{
		return;
	}

	Actions.Remove(ActionToRemove);
}

USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) const
{
	for (USAction* Action : Actions)
	{
		//if action not null and is an action class
		if (Action && Action->IsA(ActionClass))
		{
			return Action;
		}
	}

	return nullptr;
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		//if action not null and name matches start action
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				//debug messages
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);

				continue;
			}

			// Is Client? Call server RPC
			if (!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator, ActionName);
			}

			Action->StartAction(Instigator);
			return true;
		}
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		//if action not null and name matches stop action
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				Action->StopAction(Instigator);
				return true;
			}
			
		}
	}

	return false;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}


//tells unreal this is an array of UObjects and they should be replicated
	bool USActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags * RepFlags)
	{
		bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
		for (USAction* Action : Actions)
		{
			if (Action)
			{
				WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
			}
		}

		return WroteSomething;
	}

//replicates defined properties
void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}



