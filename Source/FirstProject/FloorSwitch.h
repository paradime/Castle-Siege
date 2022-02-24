// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloorSwitch : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFloorSwitch();

	/** Overlap volume for functionality to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = FloorSwitch)
	class UBoxComponent* TriggerBox;

	/** Switch for the character to step on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = FloorSwitch)
	class UStaticMeshComponent* FloorSwitch;

	/** Door to move when the floor switch is stepped on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = FloorSwitch)
	class UStaticMeshComponent* Door;

	/** Initial location for the door */
	UPROPERTY(BlueprintReadWrite, Category = FloorSwitch)
	FVector InitialDoorLocation;

	/** Initial location for the floor switch */
	UPROPERTY(BlueprintReadWrite, Category = FloorSwitch)
	FVector InitialSwitchLocation;

	FTimerHandle SwitchHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = FloorSwitch)
	float SwitchTime;

	bool bCharacterOnSwitch;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void RaiseFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = FloorSwitch)
	void LowerFloorSwitch();

	UFUNCTION(BluePrintCallable, Category = FloorSwitch)
	void UpdateDoorLocation(float Z);
	UFUNCTION(BluePrintCallable, Category = FloorSwitch)
	virtual void UpdateFloorSwitchLocation(float Z);

	void CloseDoor();
};
