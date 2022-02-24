// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Enemy.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"

// Sets default values
AMain::AMain()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create camera boom (Pulls towards player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera follows at distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(39.f, 88.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom,
	// let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	BaseTurnRate = 65.f;
	BaseLookupRate = 65.f;

	// Don't rotate when the controller rotates
	// let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 840.f, 0.f); // ...at this rotation rate.
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = .2f;

	// Player Stats
	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	bSprintKeyDown = false;
	bLMBDown = false;
	bESCDown = false;

	bAttacking = false;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;
	bHasCombatTarget = false;
	bDeathAnimationPlaying = false;

	bMovingForward = false;
	bMovingRight = false;

	InvertXCamera = 1;
	InvertYCamera = 1;
	CameraSensitivity = 1.f;
	TotalCoinsInLevel = 1;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	LoadGame(false, true);
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive())
	{
		return;
	}

	float DeltaStamina = StaminaDrainRate * DeltaTime;
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bSprintKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}
			Stamina -= DeltaStamina;
			SetMovementStatus((bMovingForward || bMovingRight)
				                  ? EMovementStatus::EMS_Sprinting
				                  : EMovementStatus::EMS_Normal);
		}
		else
		{
			// shift key up
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bSprintKeyDown)
		{
			if (Stamina - DeltaStamina <= 0)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				SetMovementStatus((bMovingForward || bMovingRight)
					                  ? EMovementStatus::EMS_Sprinting
					                  : EMovementStatus::EMS_Normal);
			}
		}
		else
		{
			// shift key up
			if (DeltaStamina + Stamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
			Stamina += DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bSprintKeyDown)
		{
			Stamina = 0;
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (DeltaStamina + Stamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		Stamina += DeltaStamina;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		break;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::SprintKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::SprintKeyUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAction("HeavySwing", IE_Pressed, this, &AMain::HeavyAttack);
	//PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUpAtRate);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);
}

bool AMain::ReadyForGameLoad()
{
	return !bDeathAnimationPlaying;
}

void AMain::DisplayLMBHint(bool visible)
{
	if (MainPlayerController)
	{
		visible ? MainPlayerController->DisplayLMBHint() : MainPlayerController->HideLMBHint();
	}
}

bool AMain::CanMove(float Value)
{
	if (!MainPlayerController)
	{
		return false;
	}
	return (Value != 0.f)
		&& (!bAttacking)
		&& IsAlive()
		&& !MainPlayerController->bPauseMenuVisible;
}

void AMain::MoveForward(float Value)
{
	bMovingForward = false;
	if (CanMove(Value))
	{
		bMovingForward = true;
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveRight(float Value)
{
	bMovingRight = false;
	if (CanMove(Value))
	{
		bMovingRight = true;
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMain::TurnAtRate(float Value)
{
	if (!CanMove(Value))
	{
		return;
	}
	AddControllerYawInput(Value * BaseTurnRate * CameraSensitivity * InvertYCamera * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Value)
{
	if (!CanMove(Value))
	{
		return;
	}
	auto AmountToAdd = Value * BaseLookupRate * CameraSensitivity * InvertXCamera * GetWorld()->GetDeltaSeconds();
	if (Value != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		auto PC = Cast<APlayerController>(GetController());
		auto CurRotation = PC->GetControlRotation();
		FRotator NewRotation(FMath::Clamp(CurRotation.Pitch + AmountToAdd, 300.f, 359.f),
		                     CurRotation.Yaw,
		                     CurRotation.Roll
		);
		PC->SetControlRotation(NewRotation);
	}
	//AddControllerPitchInput(AmountToAdd);
}

void AMain::LMBDown()
{
	bLMBDown = true;
	if (!IsAlive())
	{
		return;
	}
	if (MainPlayerController)
	{
		if (MainPlayerController->bPauseMenuVisible)
		{
			return;
		}
	}
	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this, true);
		}
	}
	else if (EquippedWeapon)
	{
		Attack();
	}
}

void AMain::ESCDown()
{
	bESCDown = true;
	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::ESCUp()
{
	bESCDown = false;
}

float AMain::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
                        AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0)
	{
		Health = 0;
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->ResetCharacter();
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount; //Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AMain::DisplayVictory()
{
	UE_LOG(LogTemp, Warning, TEXT("1"));
	if (MainPlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("2"));
		MainPlayerController->DisplayVictoryEmblem();
	}
}

void AMain::Die()
{
	if (!IsAlive())
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	SetMovementStatus(EMovementStatus::EMS_Dead);
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection("Death");
	}
	if (MainPlayerController)
	{
		MainPlayerController->DisplayDeathEmblem();
	}
	bDeathAnimationPlaying = true;
}

void AMain::Jump()
{
	if (MainPlayerController && MainPlayerController->bPauseMenuVisible)
	{
		return;
	}
	if (IsAlive())
	{
		Super::Jump();
	}
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	bDeathAnimationPlaying = false;

	if (MainPlayerController)
	{
		MainPlayerController->RemoveEnemyHealthBar();
	}
}

void AMain::SetMovementStatus(EMovementStatus status)
{
	MovementStatus = status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMain::ShowPickupLocations()
{
	for (auto location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, location, 25.f, 12, FLinearColor::Green, 5.f, 2.f);
	}
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}

void AMain::Attack()
{
	if (bAttacking || !IsAlive())
	{
		return;
	}

	bAttacking = true;
	SetInterpToEnemy(true);

	auto AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		int32 Section = FMath::RandRange(0, 1);
		float Speed = 1.f;
		FName Name = FName("");
		switch (Section)
		{
		case 0:
			Speed = 3.2f;
			Name = FName("Attack_1");
			break;
		case 1:
			Speed = 2.8f;
			Name = FName("Attack_2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_Play(CombatMontage, Speed);
		AnimInstance->Montage_JumpToSection(Name, CombatMontage);
	}
}

void AMain::HeavyAttack()
{
	if (bAttacking || !IsAlive())
	{
		return;
	}

	bAttacking = true;
	SetInterpToEnemy(true);

	auto AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		float Speed = 1.8f;
		FName Name = FName("Attack_3");
		AnimInstance->Montage_Play(CombatMontage, Speed);
		AnimInstance->Montage_JumpToSection(Name, CombatMontage);
	}
}


void AMain::AttackEnd()
{
	SetInterpToEnemy(false);
	bAttacking = false;
	if (bLMBDown)
	{
		Attack();
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector TargetLocation)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
	return FRotator(0.f, LookAtRotation.Yaw, 0.f);
}

void AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);
	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}
	FVector Location = GetActorLocation();
	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();
	for (auto Actor : OverlappingActors)
	{
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
		if (DistanceToActor < MinDistance && Enemy->IsAlive())
		{
			ClosestEnemy = Enemy;
			MinDistance = DistanceToActor;
		}
	}
	if (!(ClosestEnemy->IsAlive()))
	{
		CombatTarget = nullptr;
		bHasCombatTarget = false;
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}
	if (MainPlayerController)
	{
		MainPlayerController->DisplayEnemyHealthBar();
	}
	SetCombatTarget(ClosestEnemy);
	bHasCombatTarget = true;
}

void AMain::SwitchLevel(FName LevelName)
{
	auto World = GetWorld();
	if (World)
	{
		auto CurrentLevel = World->GetMapName();
		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame()
{
	auto SaveGame = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	if (SaveGame)
	{
		SaveGame->CharacterStats.Health = Health;
		SaveGame->CharacterStats.MaxHealth = MaxHealth;
		SaveGame->CharacterStats.Stamina = Stamina;
		SaveGame->CharacterStats.MaxStamina = MaxStamina;
		SaveGame->CharacterStats.Location = GetActorLocation();
		SaveGame->CharacterStats.Rotation = GetActorRotation();

		FString MapName = GetWorld()->GetMapName();
		MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		SaveGame->CharacterMetadata.LevelName = MapName;

		if (EquippedWeapon)
		{
			SaveGame->CharacterStats.WeaponName = EquippedWeapon->Name;
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplaySaveEmblem();
		}
		UGameplayStatics::SaveGameToSlot(SaveGame, SaveGame->PlayerName, SaveGame->UserIndex);
	}
}

void AMain::LoadGame(bool SetLocation, bool SwitchLevels)
{
	auto LoadGameRef = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	auto LoadGame = Cast<UFirstSaveGame>(
		UGameplayStatics::LoadGameFromSlot(LoadGameRef->PlayerName, LoadGameRef->UserIndex)
	);
	// if no weapons have been saved, there has been no autosave.
	if (LoadGame && LoadGame->CharacterStats.WeaponName == "")
	{
		return;
	}
	// if new game, don't load anything, but set temp game to false so loading works next time;
	if (LoadGame && LoadGame->bTempNewGame)
	{
		LoadGame->bTempNewGame = false;
		UGameplayStatics::SaveGameToSlot(LoadGame, LoadGame->PlayerName, LoadGame->UserIndex);
		return;
	}
	if (LoadGame)
	{
		/** Camera **/
		InvertXCamera = LoadGame->PlayerSettings.InvertXCamera;
		InvertYCamera = LoadGame->PlayerSettings.InvertYCamera;
		CameraSensitivity = LoadGame->PlayerSettings.CameraSensitivity;

		/** Player Stats **/
		Health = LoadGame->CharacterStats.Health;
		MaxHealth = LoadGame->CharacterStats.MaxHealth;
		Stamina = LoadGame->CharacterStats.Stamina;
		MaxStamina = LoadGame->CharacterStats.MaxStamina;
		if (SetLocation)
		{
			SetActorLocation(LoadGame->CharacterStats.Location);
			SetActorRotation(LoadGame->CharacterStats.Rotation);
		}

		SetMovementStatus(EMovementStatus::EMS_Normal);
		GetMesh()->bPauseAnims = false;
		GetMesh()->bNoSkeletonUpdate = false;

		if (WeaponStorage)
		{
			auto Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
			if (Weapons)
			{
				if (Weapons->WeaponMap.Contains(LoadGame->CharacterStats.WeaponName))
				{
					auto WepRef = Weapons->WeaponMap[LoadGame->CharacterStats.WeaponName];
					auto WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(WepRef);
					WeaponToEquip->Equip(this, false);
				}
			}
		}

		if (!SwitchLevels)
		{
			if (LoadGame->CharacterMetadata.LevelName != TEXT(""))
			{
				SwitchLevel(FName(*LoadGame->CharacterMetadata.LevelName));
			}
		}
	}
}
