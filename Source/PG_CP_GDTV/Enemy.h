// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"

#include "PaperZDAnimInstance.h"

#include "Engine/TimerHandle.h"

#include "PlayerCharacter.h"

#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class PG_CP_GDTV_API AEnemy : public APaperZDCharacter
{
	GENERATED_BODY()

public:
	// ==Components==
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* PlayerDetectorSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APlayerCharacter* FollowTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTextRenderComponent* HPText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* AttackCollisionBox;

	//AnimSeq
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperZDAnimSequence* AttackAnimSequence;

	// ==Config==
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float StopDistanceToTarget = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HitPoints = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackCoolDownInSeconds = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int AttackDamage = 25;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float AttackStunDuration = 0.3f;

	//==State==
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsStunned = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanAttack = true;

	//==Timers==
	FTimerHandle StunTimer;
	FTimerHandle AttackCooldownTimer;


	FZDOnAnimationOverrideEndSignature OnAttackOverrideEndDelegate;



	AEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DetectorOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void DetectorOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool ShouldMoveToTarget();

	void UpdateDirection(float Direction);

	void UpdateHP(int NewHP);
		
	void TakeDamage(int DamageAmount, float StunDuration);
	
	void Stun(float DurationInSeconds);
	void OnStunTimerTimeout();

	void Attack();
	void OnAttackCoolDownTimerTimeout();
	void OnAttackOverrideAnimEnd(bool Completed);

	UFUNCTION()
	void AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EnableAttackCollisionBox(bool Enabled);
};
