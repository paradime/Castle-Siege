// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstSaveGame.h"

UFirstSaveGame::UFirstSaveGame()
{
	PlayerName = TEXT("Default");
	UserIndex = 0;

	bTempNewGame = false;

	CharacterStats.WeaponName = TEXT("");
	CharacterMetadata.LevelName = TEXT("");

	PlayerSettings.InvertXCamera = 1;
	PlayerSettings.InvertYCamera = 1;
	PlayerSettings.CameraSensitivity = 1.f;
}
