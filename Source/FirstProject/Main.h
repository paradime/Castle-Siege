// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainPlayerController.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	ESS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	TArray<FVector> PickupLocations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Anims)
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anims)
	class UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class USoundCue* HitSound;

	float InterpSpeed;
	bool bInterpToEnemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat)
	bool bHasCombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Controller)
	AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	class AEnemy* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category= Combat)
	FVector CombatTargetLocation;

	/** Movement Properties */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enums)
	EMovementStatus MovementStatus;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Enums)
	EStaminaStatus StaminaStatus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float SprintingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float StaminaDrainRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MinSprintStamina;
	bool bSprintKeyDown;

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rates to scale turning functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookupRate;

	/**
	*
	* Player Stats
	*
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	/** Equipment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	TSubclassOf<AEnemy> EnemyFilter;

	UPROPERTY(EditDefaultsOnly, Category = SavedData)
	TSubclassOf<class AItemStorage> WeaponStorage;

	int32 TotalCoinsInLevel;


private:
	bool bLMBDown;
	bool bESCDown;
	bool bMovingForward;
	bool bMovingRight;

	int InvertXCamera;
	int InvertYCamera;
	float CameraSensitivity;

	bool bDeathAnimationPlaying;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for forwards/Backwards input */
	void MoveForward(float Value);

	/** Called for Side to side input */
	void MoveRight(float Value);

	/** Called via input to turn at a given rate
	* @param Value This is a normalized rate, ie. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Value);

	/** Called via input to look up/down at a given rate
	* @param Value This is a normalized rate, ie. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Value);

	void LMBDown();
	FORCEINLINE void LMBUp()
	{
		bLMBDown = false;
	}

	void ESCDown();
	void ESCUp();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const
	{
		return CameraBoom;
	}

	FORCEINLINE class UCameraComponent* GetFollowCamera() const
	{
		return FollowCamera;
	}

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	void DisplayVictory();
	void Die();

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount)
	{
		Health = (Health + Amount >= MaxHealth) ? MaxHealth : Health + Amount;
	}

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int Amount)
	{
		Coins += Amount;
	}

	/** Set Movement status and running speed */
	void SetMovementStatus(EMovementStatus status);
	FORCEINLINE void SetStaminaStatus(EStaminaStatus status)
	{
		StaminaStatus = status;
	}

	FORCEINLINE void SprintKeyDown()
	{
		bSprintKeyDown = true;
	}

	FORCEINLINE void SprintKeyUp()
	{
		bSprintKeyDown = false;
	}

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	void SetEquippedWeapon(AWeapon* WeaponToSet);

	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item)
	{
		ActiveOverlappingItem = Item;
	}

	void Attack();
	void HeavyAttack();
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	FORCEINLINE void SetInterpToEnemy(bool Interp)
	{
		bInterpToEnemy = Interp;
	}

	FORCEINLINE void SetCombatTarget(AEnemy* Target)
	{
		CombatTarget = Target;
	}

	FRotator GetLookAtRotationYaw(FVector TargetLocation);

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }
	void UpdateCombatTarget();

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetLocation, bool SwitchLevels);

	bool IsAlive() { return MovementStatus != EMovementStatus::EMS_Dead; }

	UFUNCTION(BlueprintCallable)
	bool ReadyForGameLoad();

	void DisplayLMBHint(bool visible);

	// UFUNCTION(BlueprintCallable)
	// void LoadGameNoSwitch();
private:
	bool CanMove(float Value);
};
