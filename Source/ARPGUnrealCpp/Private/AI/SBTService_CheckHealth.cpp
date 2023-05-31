// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealth.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USBTService_CheckHealth::USBTService_CheckHealth()
{
	LowHealthFraction = 0.3f; //30%
}

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(ensure(AIPawn)) //check nullptr
	{
		USAttributeComponent* AttributeComponent = USAttributeComponent::GetAttributes(AIPawn);
		if (ensure(AttributeComponent)) // check nullptr
		{
			//bool if pawn health percentage is less than LowHealthFraction value
			bool bLowHealth = ((AttributeComponent->GetHealth() / AttributeComponent->GetHealthMax()) < LowHealthFraction);

			//sets the bool value of the blackboard key
			UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
			BlackboardComponent->SetValueAsBool(LowHealthKey.SelectedKeyName, bLowHealth);
		}
	}
}


