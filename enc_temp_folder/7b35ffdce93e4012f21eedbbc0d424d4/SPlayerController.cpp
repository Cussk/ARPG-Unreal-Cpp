// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include "Blueprint/UserWidget.h"

void ASPlayerController::TogglePauseMenu()
{
	//close pause menu
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		//remove menu UI
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		//deactivate mouse, restart game controls
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
		return;
	}

	//open pause menu
	PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
	if (PauseMenuInstance)
	{
		//high z-order to put menu to front of screen
		PauseMenuInstance->AddToViewport(100);

		//activate mouse for player, deactivate game input
		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}


//bind input in player controller
void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ASPlayerController::TogglePauseMenu);
}

void ASPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}


void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateReceived.Broadcast(PlayerState);
}