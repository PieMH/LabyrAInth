// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StartMenuWidget.h"
#include "EndWidget.h"
#include "PauseMenuWidget.h"
#include "TimerWidget.h"
#include "GameFramework/Actor.h"
#include "WidgetController.generated.h"


UCLASS()
class PROGETTINOUNI_API AWidgetController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWidgetController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UStartMenuWidget> BP_StartWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UEndWidget> BP_EndWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UTimerWidget> BP_TimerWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UPauseMenuWidget> BP_PauseMenuWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY()
	UStartMenuWidget* StartMenuWidget;

	UFUNCTION()
	UStartMenuWidget* GetStartMenuWidget() const;

	UPROPERTY()
	UEndWidget* EndWidget;

	UPROPERTY()
	UTimerWidget* TimerWidget;

	UPROPERTY()
	UPauseMenuWidget* PauseMenuWidget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void ShowStartUI() const;

	void StartGame() const;

	void PauseGame() const;

	void ResumeGame() const;

	void QuitGame() const;

	static void RestartGame();
	
};