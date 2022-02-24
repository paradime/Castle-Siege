// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"

#include "AIController.h"
#include "Main.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABoss::ABoss()
{
	SlamCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SlamCollisionSphere"));
	SlamCollisionSphere->SetupAttachment(GetRootComponent());
	SlamCollisionSphere->InitSphereRadius(75.f);

	JumpCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("JumpCollisionSphere"));
	JumpCollisionSphere->SetupAttachment(GetRootComponent());
	JumpCollisionSphere->InitSphereRadius(75.f);

	GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);

	JumpDamage = 15.f;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();
}

void ABoss::ExecuteAttackAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	auto AttackChoice = FMath::RandRange(0, 2);
	FString AttackName = "Attack";
	if (AttackChoice == 0)
	{
		AttackName = "SlamAttack";
		Damage = 15;
	}
	if (AttackChoice == 1)
	{
		AttackName = "Attack";
		Damage = 20;
	}
	if (AttackChoice == 2)
	{
		return BombThrow();
	}
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, .5f);
		AnimInstance->Montage_JumpToSection(*AttackName, CombatMontage);
	}
}

void ABoss::AggroOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// does not care about aggro sphere
}

void ABoss::Die(AActor* Causer)
{
	CombatTarget->DisplayVictory();
	Super::Die(Causer);
}

void ABoss::TriggerSlamDamage()
{
	TArray<AActor*> Actors;
	TSubclassOf<AActor> ActorFilter = AMain::StaticClass();
	SlamCollisionSphere->GetOverlappingActors(Actors, ActorFilter);
	if (Actors.Num() == 0)
	{
		return;
	}
	for (auto main : Actors)
	{
		auto Main = Cast<AMain>(main);
		if (Main)
		{
			UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
		}
	}
}

void ABoss::TriggerJumpDamage()
{
	TArray<AActor*> Actors;
	TSubclassOf<AActor> ActorFilter = AMain::StaticClass();
	SlamCollisionSphere->GetOverlappingActors(Actors, ActorFilter);
	if (Actors.Num() == 0)
	{
		return;
	}
	for (auto main : Actors)
	{
		auto Main = Cast<AMain>(main);
		if (Main)
		{
			UGameplayStatics::ApplyDamage(Main, JumpDamage, AIController, this, DamageTypeClass);
		}
	}
}
