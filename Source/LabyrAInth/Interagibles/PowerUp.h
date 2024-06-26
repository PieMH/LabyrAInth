// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"

class UStatsChangerComponent;

UCLASS()
class LABYRAINTH_API APowerUp : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APowerUp();

	/** Sound to play on collect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	USoundBase* CollectSound;

	/** PowerUp the character */
	UFUNCTION(BlueprintCallable, Category = "PowerUp")
	void OnCollect(TArray<FStatsType> Stats, UStatsChangerComponent* StatsChanger);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
