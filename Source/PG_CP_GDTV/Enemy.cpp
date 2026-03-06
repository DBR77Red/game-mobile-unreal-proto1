// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerDetectorSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectorSphere"));
	PlayerDetectorSphere->SetupAttachment(RootComponent);

	HPText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HPText"));
	HPText->SetupAttachment(RootComponent);

	AttackCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollisionBox"));
	AttackCollisionBox->SetupAttachment(RootComponent);

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// binding the two functions to the Player Detector Sphere


	PlayerDetectorSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::DetectorOverlapBegin);
	PlayerDetectorSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::DetectorOverlapEnd);

	UpdateHP(HitPoints);

	OnAttackOverrideEndDelegate.BindUObject(this, &AEnemy::OnAttackOverrideAnimEnd);

	//test
	AttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackBoxOverlapBegin);

	EnableAttackCollisionBox(false);

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsAlive && FollowTarget && !IsStunned)
	{
		
		if(ShouldMoveToTarget())
		{
			//turnary operator
			float MoveDirection = (FollowTarget->GetActorLocation().X - GetActorLocation().X) > 0.0f ? 1.0f : -1.0f;
			UpdateDirection(MoveDirection);

			if (CanMove)
			{
				FVector WorldDirection = FVector(1.0f, 0.0f, 0.0f);
				AddMovementInput(WorldDirection, MoveDirection);

			}
		}
		else
		{
			//Attack
			if (FollowTarget->IsAlive)
			{
				Attack();
			}
		}
		
	}

}

void AEnemy::UpdateDirection(float MoveDirection)
{
	FRotator CurrentRotation = GetActorRotation();

	if (MoveDirection < 0.0f)
	{
		if (CurrentRotation.Yaw != 180.0f)
		{
			SetActorRotation(FRotator(CurrentRotation.Pitch, 180.0f, CurrentRotation.Roll));
		}
	}
	else if (MoveDirection > 0.0f)
	{
		if (CurrentRotation.Yaw != 0.0f)
		{
			SetActorRotation(FRotator(CurrentRotation.Pitch, 0.0f, CurrentRotation.Roll));
		}
	}
}

bool AEnemy::ShouldMoveToTarget()
{
	bool Result = false;

	if (FollowTarget)
	{
		float DistanceToTarget = abs(FollowTarget->GetActorLocation().X - GetActorLocation().X);
		Result = DistanceToTarget > StopDistanceToTarget;
	}

	return Result;
}

void AEnemy::DetectorOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Entrou na Sphere"));

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if(Player)
	{
		FollowTarget = Player;
	}

}

void AEnemy::DetectorOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Saiu na Sphere"));
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		FollowTarget = NULL;
		
	}
}

void AEnemy::UpdateHP(int NewHP)
{
	HitPoints = NewHP;

	FString Str = FString::Printf(TEXT("HP: %d"), HitPoints);
	HPText->SetText(FText::FromString(Str));
}

void AEnemy::TakeDamage(int DamageAmount, float StunDuration)
{
	if (!IsAlive) return;

	Stun(StunDuration);
	
	UpdateHP(HitPoints - DamageAmount);

	if (HitPoints <= 0)
	{
		// Enemy is dead
		UpdateHP(0);
		HPText->SetHiddenInGame(true);

		IsAlive = false;
		CanMove = false;
		CanAttack = false;

		GetAnimInstance()->JumpToNode(FName("JumpDie"), FName("Crabby_StateMachine"));

		EnableAttackCollisionBox(false);
	}
	else
	{
		// Enemy is still alive

		
		
		GetAnimInstance()->JumpToNode(FName("JumpTakeHit"), FName("Crabby_StateMachine"));

	}
}


void AEnemy::Stun(float DurationInSeconds)
{
	IsStunned = true;

	bool IsTimerAlreadyActive = GetWorldTimerManager().IsTimerActive(StunTimer);
	if (IsTimerAlreadyActive)
	{
		GetWorldTimerManager().ClearTimer(StunTimer);
	}
	
	GetWorldTimerManager().SetTimer(StunTimer, this, &AEnemy::OnStunTimerTimeout, 1.0f, false, DurationInSeconds);
	GetAnimInstance()->StopAllAnimationOverrides();
	EnableAttackCollisionBox(false);
}

void AEnemy::OnStunTimerTimeout()
{
	IsStunned = false;

}


void AEnemy::Attack()
{
	if (IsAlive && CanAttack && !IsStunned)
	{
		CanAttack = false;
		CanMove = false;

		GetAnimInstance()->PlayAnimationOverride(AttackAnimSequence, FName("DefaultSlot"), 1.0f, 0.0f, OnAttackOverrideEndDelegate);

		GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AEnemy::OnAttackCoolDownTimerTimeout, 1.0f, false, AttackCoolDownInSeconds);
	}
}

void AEnemy::OnAttackOverrideAnimEnd(bool Completed)
{
	if (IsAlive)
	{
		CanMove = true;
	}

}

void AEnemy::OnAttackCoolDownTimerTimeout()
{
	if (IsAlive)
	{
		CanAttack = true;
	}
}

void AEnemy::AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{
		//Player->TakeDamage
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, TEXT("Player Take Damage"));

	}

}

void AEnemy::EnableAttackCollisionBox(bool Enabled)
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
