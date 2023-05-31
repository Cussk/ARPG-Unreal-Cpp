// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


USBTService_CheckAttackRange::USBTService_CheckAttackRange()
{
	MaxAttackRange = 2000.0f;
}

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                            float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//Check distance between AI and pawn actor

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComponent)) //check nullptr
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject("TargetActor"));
		if (TargetActor) //check null ptr  
		{
			AAIController* MyController = OwnerComp.GetAIOwner();
			if (ensure(MyController)) //check null ptr
			{
				APawn* AIPawn = MyController->GetPawn();
				if (ensure(AIPawn)) //check null ptr
				{

					float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

					bool bWithinRange = DistanceTo < MaxAttackRange;

					//can see target
					bool bHasLOS = false;

					if (bWithinRange)
					{
						bHasLOS = MyController->LineOfSightTo(TargetActor);
					}
					

					BlackboardComponent->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bHasLOS));
				}
			}
		}
	}
}
