// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"

#include "AIController.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Critter.h"
#include "Enemy.h"
#include "Components/ArrowComponent.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (Actor_1 && Actor_2 && Actor_3 && Actor_4)
	{
		SpawnArray.Add(Actor_1);
		SpawnArray.Add(Actor_2);
		SpawnArray.Add(Actor_3);
		SpawnArray.Add(Actor_4);
	}
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	FVector Origin = SpawningBox->GetComponentLocation();

	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	if (SpawnArray.Num() == 0)
	{
		return nullptr;
	}
	int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1);
	return SpawnArray[Selection];
}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (!ToSpawn)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	FActorSpawnParameters SpawnParams;
	auto Actor = World->SpawnActor<AActor>(ToSpawn, Location, GetActorRotation(), SpawnParams);
	AEnemy* Enemy = Cast<AEnemy>(Actor);
	if (Enemy)
	{
		Enemy->SpawnDefaultController();
		AAIController* AiCont = Cast<AAIController>(Enemy->GetController());
		if (AiCont)
		{
			Enemy->AIController = AiCont;
		}
	}
}
