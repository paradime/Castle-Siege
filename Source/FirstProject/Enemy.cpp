// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "AITypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());
	AggroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), "EnemySocket");

	Health = 75.f;
	MaxHealth = 100.f;
	Damage = 10.f;

	DeathDelay = 1.f;
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;
	Home = FVector(0.f);
	HomeRotation = FRotator(0.f);

	TelegraphPlayRate = .5f;
	SwingPlayRate = 1.35f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	AIController = Cast<AAIController>(GetController());

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AggroOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AggroOnOverlapEnd);
	AggroSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);
	CombatSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	// setup combat collision
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::SwingOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::SwingOnOverlapEnd);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bOverlappingCombatSphere = false;

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;
	bHasValidTarget = false;
	Home = GetActorLocation();
	HomeRotation = GetActorRotation();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::AggroOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			MoveToTarget(Main);
			CombatTarget = Main;
		}
	}
}

void AEnemy::AggroOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->CombatTarget == this)
			{
				Main->SetCombatTarget(nullptr);
			}
			Main->SetHasCombatTarget(false);
			bHasValidTarget = false;

			CombatTarget = nullptr;
			Main->UpdateCombatTarget();

			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			ResetCharacter();
		}
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetCombatTarget(this);
			Main->SetHasCombatTarget(true);
			bHasValidTarget = true;

			Main->UpdateCombatTarget();

			bOverlappingCombatSphere = true;
			CombatTarget = Main;
			Attack();
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherComp && IsAlive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main && Main->IsAlive())
		{
			bOverlappingCombatSphere = false;
			//MoveToTarget(Main);

			if (Main->CombatTarget == this)
			{
				Main->SetCombatTarget(nullptr);
				Main->bHasCombatTarget = false;
				Main->UpdateCombatTarget();
			}
			if (Main->MainPlayerController)
			{
				USkeletalMeshComponent* MainMesh = Cast<USkeletalMeshComponent>(OtherComp);
				if (MainMesh)
				{
					Main->MainPlayerController->RemoveEnemyHealthBar();
				}
			}

			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AEnemy::SwingOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->HitParticles)
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				if (TipSocket)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						Main->HitParticles,
						TipSocket->GetSocketLocation(GetMesh()),
						FRotator(0.f),
						false
					);
				}
			}
			if (Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}

			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::SwingOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::MoveToTarget(AMain* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.f);

		FNavPathSharedPtr NavPath;
		AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void AEnemy::ActivateCollision()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_SetPlayRate(CombatMontage, SwingPlayRate);
	}
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	if (!IsAlive())
	{
		return;
	}
	if (!bHasValidTarget)
	{
		return;
	}
	if (AIController)
	{
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	}
	if (!bAttacking)
	{
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation()));
		bAttacking = true;
		ExecuteAttackAnimation();
	}
}

void AEnemy::ExecuteAttackAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, TelegraphPlayRate);
		AnimInstance->Montage_JumpToSection("Attack", CombatMontage);
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;
	float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
	if (bOverlappingCombatSphere)
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
	else
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::MoveToCombatTarget, AttackTime);
	}
}

void AEnemy::MoveToCombatTarget()
{
	if (CombatTarget && CombatTarget->IsAlive())
	{
		MoveToTarget(CombatTarget);
	}
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Pause(CombatMontage);
	}

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

void AEnemy::Die(AActor* Causer)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection("Death", CombatMontage);
	}
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	CombatCollision->SetCollisionEnabled((ECollisionEnabled::NoCollision));
	AggroSphere->SetCollisionEnabled((ECollisionEnabled::NoCollision));
	CombatSphere->SetCollisionEnabled((ECollisionEnabled::NoCollision));
	GetCapsuleComponent()->SetCollisionEnabled((ECollisionEnabled::NoCollision));

	if (CombatTarget)
	{
		CombatTarget->UpdateCombatTarget();
	}
}

float AEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
                         AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0;
		Die(DamageCauser);
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount; //Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool AEnemy::IsAlive()
{
	return EnemyMovementStatus != EEnemyMovementStatus::EMS_Dead;
}
