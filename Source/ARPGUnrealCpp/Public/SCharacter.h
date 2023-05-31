// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

//forward declare, reduce compilation time
class USpringArmComponent;
class UCameraComponent;
class USInteractionComponent;
class UAnimMontage;
class USAttributeComponent;
class UParticleSystem;
class USActionComponent;

UCLASS()
class ARPGUNREALCPP_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	// EditAnywhere - edit in BP editor and per-instance in level.
	// VisibleAnywhere - 'read-only' in editor and level. (Use for Components)
	// EditDefaultsOnly - hide variable per-instance, edit in BP editor only
	// VisibleDefaultsOnly - 'read-only' access for variable, only in BP editor (uncommon)
	// EditInstanceOnly - allow only editing of instance (eg. when placed in level)
	// --
	// BlueprintReadOnly - read-only in the Blueprint scripting (does not affect 'details'-panel)
	// BlueprintReadWrite - read-write access in Blueprints
	// --
	// Category = "" - display only for detail panels and blueprint context menu.

	UPROPERTY(VisibleAnywhere, Category = "Effects")
		FName TimeToHitParamName;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp; //allows third person camera arm

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USActionComponent* ActionComp;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void SprintStart();

	void SprintStop();

	void PrimaryAttack();

	void BlackHoleAttack();

	void Dash();

	void PrimaryInteract();

	UFUNCTION()
		void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	virtual void PostInitializeComponents() override;

	virtual FVector GetPawnViewLocation() const override;

public:	

	ASCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//console command
	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);

};
