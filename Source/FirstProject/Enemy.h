// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking UMETA(DisplayName = "Attacking"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_DefaultMAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	EEnemyMovementStatus EnemyMovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	class USphereComponent* AggroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	class AAIController* AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
	bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
	class AMain* CombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class USoundCue* SwingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	FVector Home;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	FRotator HomeRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat)
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	class UAnimMontage* CombatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bAttacking;

	/** Attack timer */
	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float AttackMinTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float AttackMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float InitialAttackTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	TSubclassOf<UDamageType> DamageTypeClass;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float DeathDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float TelegraphPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float SwingPlayRate;

	float JumpDamage;

	bool bHasValidTarget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetEnemyMovementStatus(EEnemyMovementStatus Status)
	{
		EnemyMovementStatus = Status;
	}

	UFUNCTION()
	virtual void AggroOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                 const FHitResult& SweepResult);

	UFUNCTION()
	virtual void AggroOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                  const FHitResult& SweepResult);

	UFUNCTION()
	virtual void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void SwingOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                 const FHitResult& SweepResult);

	UFUNCTION()
	virtual void SwingOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMain* Target);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UFUNCTION(BlueprintCallable)
	void Attack();
	virtual void ExecuteAttackAnimation();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	void MoveToCombatTarget();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	virtual void Die(AActor* Causer);
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	bool IsAlive();

	UFUNCTION(BlueprintImplementableEvent)
	void ResetCharacter();
protected:
	FORCEINLINE void Disappear() { Destroy(); }
};
