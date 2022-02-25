// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			RemoveEnemyHealthBar();
		}

		EnemyHealthBar->SetAlignmentInViewport(FVector2D(0.f));
	}
	AddAndHideToViewport(WAutoSaveEmblem, &AutoSaveEmblem);
	AddAndHideToViewport(WDeathEmblem, &DeathEmblem);
	AddAndHideToViewport(WVictoryEmblem, &VictoryEmblem);
	AddAndHideToViewport(WLMBHint, &LMBHint);
	AddAndHideToViewport(WPauseMenu, &PauseMenu);
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;
		PositionInViewport.X -= 50.f;

		FVector2D SizeInViewport(200.f, 15.f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
		FInputModeGameAndUI InputMode;
		bShowMouseCursor = true;
		SetInputMode(InputMode);
	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = false;
		FInputModeGameOnly InputMode;
		bShowMouseCursor = false;
		SetInputMode(InputMode);
	}
	if (DeathEmblem)
	{
		DeathEmblem->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::TogglePauseMenu()
{
	bPauseMenuVisible ? RemovePauseMenu() : DisplayPauseMenu();
}

void AMainPlayerController::DisplaySaveEmblem()
{
	if (AutoSaveEmblem)
	{
		AutoSaveEmblem->SetVisibility(ESlateVisibility::Visible);
		FTimerHandle SaveTimer;
		GetWorld()->GetTimerManager().SetTimer(SaveTimer, this, &AMainPlayerController::HideSaveEmblem, 2.f);
	}
}

void AMainPlayerController::HideSaveEmblem()
{
	if (AutoSaveEmblem)
	{
		AutoSaveEmblem->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayLMBHint()
{
	if (LMBHint) LMBHint->SetVisibility(ESlateVisibility::Visible);
}

void AMainPlayerController::HideLMBHint()
{
	if (LMBHint) LMBHint->SetVisibility(ESlateVisibility::Hidden);
}

void AMainPlayerController::AddAndHideToViewport(TSubclassOf<UUserWidget> SubUWidget, UUserWidget** UWidget)
{
	if (SubUWidget)
	{
		*UWidget = CreateWidget<UUserWidget>(this, SubUWidget);
		if (*UWidget)
		{
			(*UWidget)->AddToViewport();
			(*UWidget)->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("A Widget it not set"));
	}
}
