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


UCLASS()
class ARPGUNREALCPP_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Attack")
		TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
		UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	FTimerHandle TimerHandle_PrimaryAttack;


public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	
	UPROPERTY(VisibleAnywhere) //exposes property in editor
	USpringArmComponent* SpringArmComp; //allows third person camera arm

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void PrimaryAttack();
	void PrimaryAttack_TimeElapsed();

	void PrimaryInteract();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
