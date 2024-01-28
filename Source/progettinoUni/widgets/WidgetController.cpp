// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetController.h"

#include "TimerWidget.h"
#include "EndWidget.h"
#include "StartMenuWidget.h"
#include "Components/TileView.h"

// Sets default values
AWidgetController::AWidgetController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AWidgetController::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World) { return; }

	if (BP_GenMapWidget)
	{
		GenMapWidget = CreateWidget<UGenMapWidget>(World, BP_GenMapWidget);
		GenMapWidget->WidgetController = this;
	}
	if (BP_StartWidget)
	{
		StartMenuWidget = CreateWidget<UStartMenuWidget>(World, BP_StartWidget);
		StartMenuWidget->WidgetController = this;
	}
	if (BP_EndWidget)
	{
		EndWidget = CreateWidget<UEndWidget>(World, BP_EndWidget);
		EndWidget->WidgetController = this;
	}
	if (BP_TimerWidget)
	{
		TimerWidget = CreateWidget<UTimerWidget>(World, BP_TimerWidget);
		TimerWidget->WidgetController = this;
	}
	if (BP_PauseMenuWidget)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(World, BP_PauseMenuWidget);
		PauseMenuWidget->WidgetController = this;
	}

	ShowGenMapUI();
	
}


void AWidgetController::ShowGenMapUI() const
{
	if (GenMapWidget)
	{
		GenMapWidget->AddToViewport(0);
	}
}

void AWidgetController::ShowStartUI() const
{
	if (GenMapWidget)
	{
		GenMapWidget->RemoveFromParent();
	}
	if (StartMenuWidget)
	{
		StartMenuWidget->AddToViewport(0);
	}
}

void AWidgetController::StartGame() const
{
	if (StartMenuWidget)
	{
		StartMenuWidget->RemoveFromParent();
	}
	if (TimerWidget)
	{
		TimerWidget->AddToViewport(0);
	}
}

void AWidgetController::PauseGame() const
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->AddToViewport(0);
	}
}

void AWidgetController::ResumeGame() const
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
	}
}

void AWidgetController::QuitGame() const
{
	if (PauseMenuWidget)
	{
		
	}
}

void AWidgetController::RestartGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

UStartMenuWidget* AWidgetController::GetStartMenuWidget() const
{
	if (StartMenuWidget) return StartMenuWidget;
	else return nullptr;
}

// Called every frame
void AWidgetController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
