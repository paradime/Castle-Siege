// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FirstSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category="SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category="SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category="SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category="SaveGameData")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category="SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category="SaveGameData")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category="SaveGameData")
	FString WeaponName;
};


USTRUCT(BlueprintType)
struct FCharacterMetadata
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveGameData")
	FString LevelName;
};


USTRUCT(BlueprintType)
struct FPlayerSettings
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveSettingsGameData")
	int InvertXCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveSettingsGameData")
	int InvertYCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveSettingsGameData")
	float CameraSensitivity;
};

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API UFirstSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UFirstSaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Basic)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Basic)
	int UserIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Basic)
	bool bTempNewGame;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Basic)
	FCharacterStats CharacterStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Basic)
	FPlayerSettings PlayerSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Basic)
	FCharacterMetadata CharacterMetadata;

};
