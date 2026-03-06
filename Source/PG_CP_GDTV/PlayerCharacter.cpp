// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "DrawDebugHelpers.h"

#include "Enemy.h"

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

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::JumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpEnded);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &APlayerCharacter::JumpEnded);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);

		//EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &APlayerCharacter::Shoot);
		
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	float MoveActionValue = Value.Get<float>();
	
	if (IsAlive && CanMove)
	{
		
		FVector Direction = FVector(1.0f, 0.0f, 0.0f);
		AddMovementInput(Direction, MoveActionValue);
		UpdateDirection(MoveActionValue);

	}
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
	if (IsAlive && CanMove)
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
	if (IsAlive && CanAttack)
	{
		CanAttack = false;
		CanMove = false;

		//EnableAttackCollisionBox(true);

		GetAnimInstance()->PlayAnimationOverride(AttackAnimSequence, FName("DefaultSlot"), 1.0f, 0.0f, OnAttackOverrideEndDelegate);

	}


}

void APlayerCharacter::OnAttackOverrideAnimationAnimEnd(bool Completed)
{
	CanAttack = true;
	CanMove = true;

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

	UpdateHP(HitPoints - DamageAmount);

	if (HitPoints <= 0)
	{
		////plyer dead
		UpdateHP(0);

		IsAlive = false;
		CanMove = false;
		CanAttack = false;

		GetAnimInstance()->JumpToNode(FName("JumpDie"));
		EnableAttackCollisionBox(false);
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
}