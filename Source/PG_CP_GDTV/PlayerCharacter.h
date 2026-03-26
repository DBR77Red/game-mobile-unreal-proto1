// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/TimerHandle.h"

#include "Sound/SoundBase.h"

#include "InputActionValue.h"


#include "GameFramework/Controller.h"

#include "PaperZDAnimInstance.h"
//for finding the name of the StateMachine
#include "PaperZDAnimBPGeneratedClass.h"
#include "AnimNodes/PaperZDAnimNode_StateMachine.h"
//

#include "PlayerHUD.h"
#include "CrustyPirateGameInstance.h"
#include "PauseMenuWidget.h"

#include "CollectableItem.h"


#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PG_CP_GDTV_API APlayerCharacter : public APaperZDCharacter
{
	GENERATED_BODY()

public:
	// === Components ===
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera; 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* AttackCollisionBox;

	// === Inputs ===
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* QuitGame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* PauseAction;

	//AnimSeq
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperZDAnimSequence* AttackAnimSequence;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPlayerHUD* PlayerHUDWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UPauseMenuWidget> PauseMenuWidgetInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCrustyPirateGameInstance* MyGameInstance;

	//Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* ItemPickupSound;

	// === State ===
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", meta = (ToolTip = "Whether the player is alive or has been defeated"))
	bool IsAlive = true; 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", meta = (ToolTip = "Whether the player is active or not, used for level transition"))
	bool IsActive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", meta = (ToolTip = "Whether the player is alive or has been defeated"))
	bool IsStunned = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", meta = (ToolTip = "Whether the player is allowed to move"))
	bool CanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", meta = (ToolTip = "Whether the player is allowed to move"))
	bool CanAttack = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int HitPoints = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackDamage = 25;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackStunDuration = 0.3f;

	FZDOnAnimationOverrideEndSignature OnAttackOverrideEndDelegate;

	FTimerHandle StunTimer;

	FTimerHandle RestartTimer;


	APlayerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override; 

	void Move(const FInputActionValue& Value);
	void MoveEnded(const FInputActionValue& Value);
	void JumpStarted(const FInputActionValue& Value);
	void JumpEnded(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

	void Quit(const FInputActionValue& Value);
	void Pause(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void OpenPauseMenu();


	void UpdateDirection(float MoveDirection);

	void OnAttackOverrideAnimationAnimEnd(bool Completed);

	UFUNCTION()
	void AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EnableAttackCollisionBox(bool Enabled);

	void TakeDamage(int DamageAmount, float StunDuration);
	void UpdateHP(int NewHP);

	void Stun(float DurationInSeconds);
	void OnStunTimerTimeout();

	void CollectItem(CollectableType ItemType);

	void OnRestartTimerTimeout();

	
	UFUNCTION(BlueprintCallable)
	void Deactivate();
};
