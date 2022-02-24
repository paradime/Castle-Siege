// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FloorSwitch.h"
#include "FloorTwoSwitch.generated.h"

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API AFloorTwoSwitch : public AFloorSwitch
{
	GENERATED_BODY()

public:
	AFloorTwoSwitch();
	/** Overlap volume for functionality to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = FloorSwitch)
	class UBoxComponent* SecondTriggerBox;

	/** Switch for the character to step on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = FloorSwitch)
	class UStaticMeshComponent* SecondFloorSwitch;

	/** Initial location for the floor switch */
	UPROPERTY(BlueprintReadWrite, Category = FloorSwitch)
	FVector InitialSecondSwitchLocation;

protected:
	virtual void BeginPlay() override;
public:
	virtual void UpdateFloorSwitchLocation(float Z) override;
};
