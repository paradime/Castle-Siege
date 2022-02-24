// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Teleport.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API ATeleport : public AItem
{
	GENERATED_BODY()

public:
	ATeleport();

	UPROPERTY(EditAnywhere, Category=Teleport, meta = (MakeEditWidget = "true"))
	FVector Destination;
	UPROPERTY(EditAnywhere, Category=Teleport, meta = (MakeEditWidget = "true"))
	FRotator DestinationRotation;

public:
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult) override;
};
