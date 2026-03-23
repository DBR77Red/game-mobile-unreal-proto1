// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

void UPauseMenuWidget::ResumeGame()
{
	UGameplayStatics::SetGamePaused(this, false);

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}

	RemoveFromParent();
}

void UPauseMenuWidget::OnResumeClicked()
{
	ResumeGame();
}

void UPauseMenuWidget::OnRestartLevelClicked()
{
	ResumeGame();
	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}

void UPauseMenuWidget::OnQuitToMenuClicked()
{
	ResumeGame();
	UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UPauseMenuWidget::OnQuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

FReply UPauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::Escape || Key == PauseKey)
	{
		ResumeGame();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
