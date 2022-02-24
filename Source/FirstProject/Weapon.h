// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Pickup UMETA(DisplayName = Pickup),
	EWS_Equipped UMETA(DisplayName = Equipped),
	EWS_Max UMETA(DisplayName = DefaultMAX)
};

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	UPROPERTY(EditDefaultsOnly, Category=SavedData)
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
	EWeaponState WeaponState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SkeletalMesh)
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	USoundCue* SwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	TSubclassOf<UDamageType> DamageTypeClass;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	AController* WeaponInstigator;

private:
	float AttackDamage;

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                   const FHitResult& SweepResult);

	UFUNCTION()
	void CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Equip(class AMain* Char, bool SaveGame);

	FORCEINLINE void SetWeaponState(EWeaponState State)
	{
		WeaponState = State;
	}

	FORCEINLINE EWeaponState GetWeaponState()
	{
		return WeaponState;
	}

	FORCEINLINE void SetInstigator(AController* Inst)
	{
		WeaponInstigator = Inst;
	}

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateHeavyCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();
};
