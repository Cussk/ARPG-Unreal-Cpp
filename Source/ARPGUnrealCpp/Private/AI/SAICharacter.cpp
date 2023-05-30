// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TimeToHitParamName = "TimeToHit";
}



void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//pawn scene delegate
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);

	//health change delegate
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{

		if(InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		//if no health bar, create one
		if (ActiveHealthBar == nullptr)
		{
			//creates healthbar widget
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar) //check nullptr
			{
				//attach to this instance of AICharacter
				ActiveHealthBar->AttachedActor = this;

				//add healthbar to screen
				ActiveHealthBar->AddToViewport();
			}
		}

		//call hit flash
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		if (NewHealth <= 0)
		{
			//stop Behavior Tree
			AAIController* AIController = Cast<AAIController>(GetController());
			if (AIController)
			{
				AIController->GetBrainComponent()->StopLogic("Killed");
			}

			//ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");
			
			//set lifespan until destroy called
			SetLifeSpan(10.0f);
		}
	}
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController) //check null
	{
		AIController->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);

	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}


