// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Events.h"
#include "InputCoreTypes.h"
#include "PauseMenuWidget.generated.h"

/**
 * Pause/exit menu widget. Create a UMG Blueprint subclass of this class and
 * wire each button's OnClicked event to the corresponding UFUNCTION below.
 */
UCLASS()
class PG_CP_GDTV_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Called by the Resume button — unpauses and removes the widget. */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	void OnResumeClicked();

	/** Called by the Restart button — reloads the current level. */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	void OnRestartLevelClicked();

	/** Called by the Quit to Menu button — opens the MainMenu level. */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	void OnQuitToMenuClicked();

	/** Called by the Quit Game button — exits the application. */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	void OnQuitGameClicked();

	void ResumeGame();

	// Set by PlayerCharacter::OpenPauseMenu so the widget knows which key to treat as "pause/resume"
	UPROPERTY(BlueprintReadWrite)
	FKey PauseKey = EKeys::Escape;

protected:
	// Required so Slate routes keyboard events to this widget
	virtual bool NativeSupportsKeyboardFocus() const override { return true; }

	// Catch Escape or the pause key while the widget has focus (works even when game is paused)
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
