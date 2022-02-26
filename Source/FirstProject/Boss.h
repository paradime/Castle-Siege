// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Boss.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API ABoss : public AEnemy
{
	GENERATED_BODY()

public:
	ABoss();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CombatMechanics)
	USphereComponent* SlamCollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CombatMechanics)
	USphereComponent* JumpCollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatMechanics)
	USoundCue* SlamSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatMechanics)
	TSubclassOf<class AExplosive> ExplosiveClass;

protected:
	virtual void BeginPlay() override;
public:
	virtual void ExecuteAttackAnimation() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BombThrow();

	virtual void AggroOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void Die(AActor* Causer) override;

	UFUNCTION(BlueprintCallable)
	void TriggerSlamDamage();

	UFUNCTION(BlueprintCallable)
	void TriggerJumpDamage();

	UFUNCTION(BlueprintCallable)
	void CreateBomb();

	private:
	void TriggerJumpSound();
};
