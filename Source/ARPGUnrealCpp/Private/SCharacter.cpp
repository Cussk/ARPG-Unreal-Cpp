// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp"); //adds component to character
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent); //attaches to root of character

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp); //attaches camera to spring arm

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;

	TimeToHitParamName = "TimeToHit";
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//health change delegate
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

//changes pawn eye view from character mesh to camera
FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward); //binds movement to character calling MoveForward component, specify input in editor with "MoveForward" string
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);

	// Used generic name 'SecondaryAttack' for binding
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

}

//console command
void ASCharacter::HealSelf(float Amount /* = 100 */)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRotation = GetControlRotation(); // gets the rotation of camera view

	ControlRotation.Pitch = 0.0f; //prevents pitch from affecting movement
	ControlRotation.Roll = 0.0f; //prevents roll from affecting movement

	AddMovementInput(ControlRotation.Vector(), Value); //move forward in direction of camera view
	
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRotation = GetControlRotation(); // gets the rotation of camera view

	ControlRotation.Pitch = 0.0f; //prevents pitch from affecting movement
	ControlRotation.Roll = 0.0f; //prevents roll from affecting movement

	FVector RightVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);
}

void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}

void ASCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");
}


void ASCharacter::BlackHoleAttack()
{
	ActionComp->StartActionByName(this, "SecondaryAttack");
}

void ASCharacter::Dash()
{
	ActionComp->StartActionByName(this, "Dash");
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp) // checks null
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	//if damaged do hit flash material
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
	}

	//char health lees equal 0 and was damaged not healed
	if(NewHealth <= 0.0f && Delta < 0.0f)
	{
		//cast to player controller, disable input
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		DisableInput(PlayerController);
	}
}