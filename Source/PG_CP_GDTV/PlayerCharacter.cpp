// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "DrawDebugHelpers.h"

#include "Enemy.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT ("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	AttackCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollisionBox"));
	AttackCollisionBox->SetupAttachment(RootComponent);

	}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UPaperZDAnimBPGeneratedClass* AnimClass = Cast<UPaperZDAnimBPGeneratedClass>(GetAnimInstance()->GetClass());
	if (AnimClass)
	{
		FPaperZDAnimationBaseContext Ctx(GetAnimInstance());
		for (FPaperZDAnimNode_StateMachine* SM : AnimClass->GetStateMachineNodes(GetAnimInstance()))
		{
			UE_LOG(LogTemp, Warning, TEXT("StateMachine name: %s"), *SM->GetMachineName().ToString());
		}
	}

	//Add Input Mapping Context
	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);

		}
	}

	OnAttackOverrideEndDelegate.BindUObject(this, &APlayerCharacter::OnAttackOverrideAnimationAnimEnd);
	AttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::AttackBoxOverlapBegin);
	EnableAttackCollisionBox(false);

	
	MyGameInstance = Cast<UCrustyPirateGameInstance>(GetGameInstance());
	if (MyGameInstance)
	{
		HitPoints = MyGameInstance->PlayerHP;

		if (MyGameInstance->SkullCount > 0)
		{
			JumpMaxCount = FMath::Min(MyGameInstance->SkullCount + 1, 3);
		}
	}

	
	if (PlayerHUDClass)
	{

		PlayerHUDWidget = CreateWidget<UPlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0), PlayerHUDClass);
		if (PlayerHUDWidget)
		{

			PlayerHUDWidget->AddToPlayerScreen();
			
			PlayerHUDWidget->SetHP(HitPoints);
			PlayerHUDWidget->SetDiamonds(MyGameInstance->CollectedDiamontCount);
			PlayerHUDWidget->SetLevel(MyGameInstance->CurrentLevelIndex);
		}

	}


}

void APlayerCharacter::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up input actions bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerCharacter::MoveEnded);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::JumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpEnded);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &APlayerCharacter::JumpEnded);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);

		EnhancedInputComponent->BindAction(QuitGame, ETriggerEvent::Started, this, &APlayerCharacter::Quit);

		if (PauseAction)
		{
			EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &APlayerCharacter::Pause);
		}

		//EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &APlayerCharacter::Shoot);
		
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	float HorizontalInput = Value.Get<FVector2D>().X;

	if (IsAlive && CanMove && !IsStunned)
	{
		if (HorizontalInput != 0.0f)
		{
			FVector Direction = FVector(1.0f, 0.0f, 0.0f);
			AddMovementInput(Direction, HorizontalInput);
			UpdateDirection(HorizontalInput);
		}
	}
}

void APlayerCharacter::MoveEnded(const FInputActionValue& Value)
{
}

void APlayerCharacter::UpdateDirection(float MoveDirection)
{
	FRotator CurrentRotation = Controller->GetControlRotation();
	if (MoveDirection < 0.0f)
	{
		if (CurrentRotation.Yaw != 180.0f)
		{
			Controller->SetControlRotation(FRotator(CurrentRotation.Pitch, 180.0f, CurrentRotation.Roll));
		}
	}
	else if (MoveDirection > 0.0f)
	{
		if (CurrentRotation.Yaw != 0.0f)
		{
			Controller->SetControlRotation(FRotator(CurrentRotation.Pitch, 0.0f, CurrentRotation.Roll));
		}
	}


}

void APlayerCharacter::JumpStarted(const FInputActionValue& Value)
{
	if (IsAlive && CanMove && !IsStunned)
	{
		Jump();
	}

}

void APlayerCharacter::JumpEnded(const FInputActionValue& Value)
{
	StopJumping();
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (IsAlive && CanAttack && !IsStunned)
	{
		CanAttack = false;
		CanMove = false;

		//EnableAttackCollisionBox(true);

		GetAnimInstance()->PlayAnimationOverride(AttackAnimSequence, FName("DefaultSlot"), 1.0f, 0.0f, OnAttackOverrideEndDelegate);

	}


}

void APlayerCharacter::Quit(const FInputActionValue& Value)
{

	//Passou no curso
	// UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
	
	//pesquisei online
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);

}

void APlayerCharacter::OnAttackOverrideAnimationAnimEnd(bool Completed)
{
	if (IsActive && IsAlive)
	{
		CanAttack = true;
		CanMove = true;
	}


	//EnableAttackCollisionBox(false);

}

void APlayerCharacter::AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{

	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	if (Enemy)
	{
		Enemy->TakeDamage(AttackDamage, AttackStunDuration);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, Enemy->GetName());
	}


}

UFUNCTION(BlueprintCallable)
void APlayerCharacter::EnableAttackCollisionBox(bool Enabled)
{

	if (Enabled)
	{

		AttackCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttackCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	else
	{
		AttackCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttackCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}


}


void APlayerCharacter::TakeDamage(int DamageAmount, float StunDuration)
{
	if (!IsAlive) return;
	if (!IsActive) return;

	Stun(StunDuration);

	UpdateHP(HitPoints - DamageAmount);

	if (HitPoints <= 0)
	{
		////player dead
		UpdateHP(0);

		IsAlive = false;
		CanMove = false;
		CanAttack = false;

		GetAnimInstance()->JumpToNode(FName("JumpDie"));
		EnableAttackCollisionBox(false);

		float RestartDelay = 3.0f;
		GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerCharacter::OnRestartTimerTimeout, 1.0f, false, RestartDelay);
	}
	else
	{
		//player lives
		GetAnimInstance()->JumpToNode(FName("JumpTakeHit"));


	}

}

void APlayerCharacter::UpdateHP(int NewHP)
{
	HitPoints = NewHP; 
	MyGameInstance->SetPlayerHP(HitPoints);
	PlayerHUDWidget->SetHP(HitPoints);


}

void APlayerCharacter::Stun(float DurationInSeconds)
{
	IsStunned = true;

	bool IsTimerAlreadyActive = GetWorldTimerManager().IsTimerActive(StunTimer);
	if (IsTimerAlreadyActive)
	{
		GetWorldTimerManager().ClearTimer(StunTimer);
	}

	GetWorldTimerManager().SetTimer(StunTimer, this, &APlayerCharacter::OnStunTimerTimeout, 1.0f, false, DurationInSeconds);
	GetAnimInstance()->StopAllAnimationOverrides();
	EnableAttackCollisionBox(false);
}

void APlayerCharacter::OnStunTimerTimeout()
{
	IsStunned = false;

}

void APlayerCharacter::CollectItem(CollectableType ItemType)
{
	UGameplayStatics::PlaySound2D(GetWorld(), ItemPickupSound);

	switch (ItemType)
	{
		case CollectableType::HealthPotion:
		{
			int HealAmount = 25;
			
			// In case I want to limit the number of hit points to 100
			UpdateHP(FMath::Min(HitPoints + HealAmount, 100));

			// Mostrado no curso
			//UpdateHP(HitPoints + HealAmount);

		}break;

		case CollectableType::Diamond:
		{
			MyGameInstance->AddDiamond(2);
			PlayerHUDWidget->SetDiamonds(MyGameInstance->CollectedDiamontCount);

		}break;


		case CollectableType::DoubleJumpUpgrade:
		{
			MyGameInstance->SkullCount++;
			MyGameInstance->IsDoubleJumpUnlocked = true;
			JumpMaxCount = FMath::Min(MyGameInstance->SkullCount + 1, 3);

		}break;

		default:
		{


		}break;

	}
}


void APlayerCharacter::OnRestartTimerTimeout()
{
	MyGameInstance->RestartGame();
}

void APlayerCharacter::Pause(const FInputActionValue& Value)
{
	if (IsValid(PauseMenuWidgetInstance) && PauseMenuWidgetInstance->IsInViewport())
	{
		PauseMenuWidgetInstance->ResumeGame();
	}
	else
	{
		OpenPauseMenu();
	}
}

void APlayerCharacter::OpenPauseMenu()
{
	if (!PauseMenuWidgetClass) return;
	if (IsValid(PauseMenuWidgetInstance) && PauseMenuWidgetInstance->IsInViewport()) return; // already open

	APlayerController* PC = Cast<APlayerController>(Controller);
	PauseMenuWidgetInstance = CreateWidget<UPauseMenuWidget>(PC, PauseMenuWidgetClass);
	if (PauseMenuWidgetInstance)
	{
		// Tell the widget which key should act as pause/resume so it can handle it
		// while the game is paused (Enhanced Input won't fire when paused, but NativeOnKeyDown will).
		if (InputMappingContext && PauseAction)
		{
			for (const FEnhancedActionKeyMapping& Mapping : InputMappingContext->GetMappings())
			{
				if (Mapping.Action == PauseAction)
				{
					PauseMenuWidgetInstance->PauseKey = Mapping.Key;
					break;
				}
			}
		}

		PauseMenuWidgetInstance->AddToPlayerScreen();
		UGameplayStatics::SetGamePaused(this, true);
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(PauseMenuWidgetInstance->TakeWidget());
			PC->SetInputMode(InputMode);
		}
	}
}


void APlayerCharacter::Deactivate()
{
	if (IsActive)
	{
		IsActive = false;
		CanAttack = false;
		CanMove = false;


		GetCharacterMovement()->StopMovementImmediately();




	}

}