// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"
#include "SWorldUserWidget.h"
#include "Blueprint/UserWidget.h"

//toggle debug draws with console
static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("s.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interact Component."), ECVF_Cheat);

USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	TraceDistance = 500.0f;
	CollisionChannel = ECC_WorldDynamic;
}

void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}


void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//only run if local player client
	APawn* MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}	
}

void USInteractionComponent::FindBestInteractable()
{
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	TArray<FHitResult> Hits;

	FCollisionShape Sphere;
	Sphere.SetSphere(TraceRadius);

	//sphere sweep to find collision
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Sphere);

	//line color green if found blocking hit, red if none
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	// Clear ref before trying to fill
	FocusedActor = nullptr;

	for (FHitResult Hit : Hits)
	{
		if (bDebugDraw)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 0.0f); //eyesight sphere trace
		}

		AActor* HitActor = Hit.GetActor();
		if (HitActor) //checking for null
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				FocusedActor = HitActor;

				break;
			}
		}

	}

	if (FocusedActor)
	{
		//lazy load creating widget
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if (DefaultWidgetInstance)//check null
		{
			//attach widget to focused actor
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			//if not in viewport already add it
			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else //no longer have focused actor
	{
		if (DefaultWidgetInstance) //check null
		{
			//remove from viewport
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 0.0f, 0, 2.0f); //eyesight line trace
	}
}

void USInteractionComponent::PrimaryInteract()
{
	//call interact through server
	ServerInteract(FocusedActor);
}

//interact with object server call
void USInteractionComponent::ServerInteract_Implementation(AActor* InFocus)
{
	if (InFocus == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focus Actor to interact.");
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner()); //changes type actor to pawn

	ISGameplayInterface::Execute_Interact(InFocus, MyPawn);
}


