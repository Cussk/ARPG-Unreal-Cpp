// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class ARPGUNREALCPP_API ASPowerupActor : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Powerup")
		float RespawnTime;

	UFUNCTION()
		void ShowPowerup();

	void HideAndCooldownPowerup();

	void SetPowerupState(bool bNewIsActive);

	UPROPERTY(VisibleAnywhere, Category = "Components")
		USphereComponent* SphereComp;

public:

	void Interact_Implementation(APawn* InstigatorPawn) override;

public:

	ASPowerupActor();

};
