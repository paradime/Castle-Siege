// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorTwoSwitch.h"

#include "Components/BoxComponent.h"

AFloorTwoSwitch::AFloorTwoSwitch()
{
	SecondTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SecondBox"));
	SecondTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SecondTriggerBox->SetCollisionObjectType(ECC_WorldStatic);
	SecondTriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	SecondTriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SecondTriggerBox->SetupAttachment(GetRootComponent());

	SecondFloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondSwitch"));
	SecondFloorSwitch->SetupAttachment(SecondTriggerBox);
}

void AFloorTwoSwitch::BeginPlay()
{
	Super::BeginPlay();

	InitialSecondSwitchLocation = SecondFloorSwitch->GetComponentLocation();
	SecondTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	SecondTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);
}

void AFloorTwoSwitch::UpdateFloorSwitchLocation(float Z)
{
	Super::UpdateFloorSwitchLocation(Z);
	FVector NewLocation = InitialSecondSwitchLocation;
	NewLocation.Z += Z;
	SecondFloorSwitch->SetWorldLocation(NewLocation);
}
