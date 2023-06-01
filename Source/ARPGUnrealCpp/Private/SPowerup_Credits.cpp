// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Credits.h"

#include "SPlayerState.h"


ASPowerup_Credits::ASPowerup_Credits()
{
		CreditsAmount = 80;
}

void ASPowerup_Credits::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn)) //if not the instigator pawn return early
	{
		return;
	}

	ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (PlayerState) //check null
	{
		PlayerState->AddCredits(CreditsAmount);
		HideAndCooldownPowerup();
	}
}


