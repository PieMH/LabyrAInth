// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralMap.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProceduralMap::AProceduralMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	FloorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMeshComponent"));
	FloorMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProceduralMap::BeginPlay()
{
	Super::BeginPlay();

	// to spawn a default floor and a default amount of objects
	ProgrammaticallyPlaceFloor();
	SpawnObjects();
}

// Called every frame
void AProceduralMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/**
 * Scale the floor according to the FloorWidth and FloorLength variables
 * then calls PlaceWalls() to place the walls on the edges of the floor
 */
void AProceduralMap::ProgrammaticallyPlaceFloor()
{
	AProceduralMap* ProceduralMapBPActor = Cast<AProceduralMap>(UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralMap::StaticClass()));
	if (ProceduralMapBPActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("BP_Floor is null!"));
		return;
	}
	// scale the actor floor
	ProceduralMapBPActor->SetActorScale3D(FVector(FloorWidth / 100, FloorLength / 100, 1.f));  // remember these values are in m

	PlaceWalls();
}

/**
 * place the walls on the edges of the floor
 */
void AProceduralMap::PlaceWalls()
{
	CleanObject(BP_Wall);  // destroy all the walls in the scene
	
	constexpr int32 WallThickness = 100;  // thickness of the edge's walls is 100cm or 0.1m in scale language
	constexpr int32 WallHeight = 300;  // 300cm or 3m in scale language
	
	// left wall
	FVector LeftWallLocation = FVector(0, -((FloorLength / 2) + (WallThickness / 2)), WallHeight / 2);  // we need to move of half the thickness of a wall to the left to not occupy space on the floor, and let the wall stay on the exact edge
	FRotator LeftWallRotation = FRotator(0, 0, 0);
	FVector LeftWallScale = FVector(FloorWidth / 100, 1.0f, WallHeight / 100); // we need to divide by 100 because the positions of the world are in cm and we need to convert it to m for the scale
	FTransform LeftWallTransform(LeftWallRotation, LeftWallLocation, LeftWallScale);
	AActor* LeftWall = GetWorld()->SpawnActor<AActor>(BP_Wall, LeftWallTransform);
	LeftWall->SetActorScale3D(LeftWallScale);
	
	// right wall
	FVector RightWallLocation = FVector(0, (FloorLength / 2) + (WallThickness / 2), WallHeight / 2);
	FRotator RightWallRotation = FRotator(0, 0, 0);
	FVector RightWallScale = FVector(FloorWidth / 100, 1.0f,  WallHeight / 100);
	FTransform RightWallTransform(RightWallRotation, RightWallLocation, RightWallScale);
	AActor* RightWall = GetWorld()->SpawnActor<AActor>(BP_Wall, RightWallTransform);
	RightWall->SetActorScale3D(RightWallScale);
	
	// top wall
	FVector TopWallLocation = FVector((FloorWidth / 2) + (WallThickness / 2), 0, WallHeight / 2);
	FRotator TopWallRotation = FRotator(0, 0, 0);
	FVector TopWallScale = FVector(1.0f, (FloorLength / 100) + (WallThickness * 2 / 100), WallHeight / 100);  // we need to add the thickness of two edge's walls to the width of the top and bottom walls to close the gap between the left and right walls
	FTransform TopWallTransform(TopWallRotation, TopWallLocation, TopWallScale);
	AActor* TopWall = GetWorld()->SpawnActor<AActor>(BP_Wall, TopWallTransform);
	TopWall->SetActorScale3D(TopWallScale);
	
	// bottom wall
	FVector BottomWallLocation = FVector(-((FloorWidth / 2) + (WallThickness / 2)), 0, WallHeight / 2);
	FRotator BottomWallRotation = FRotator(0, 0, 0);
	FVector BottomWallScale = FVector(1.0f, (FloorLength / 100) + (WallThickness * 2 / 100), WallHeight / 100);
	FTransform BottomWallTransform(BottomWallRotation, BottomWallLocation, BottomWallScale);
	AActor* BottomWall = GetWorld()->SpawnActor<AActor>(BP_Wall, BottomWallTransform);
	BottomWall->SetActorScale3D(BottomWallScale);
}

/**
 * Spawn all the objects in the map, each according to its amount
 * Before spawning the objects it destroys all the objects of the same type in the scene
 */
void AProceduralMap::SpawnObjects() const
{
	CleanObject(BP_Archway);
	CleanObject(BP_Pillar);
	CleanObject(BP_MovableBlocks);
	CleanObject(BP_Coin);
	CleanObject(BP_Gem);
	ProcedurallySpawnObjects(BP_Archway, ArchwayAmount);
	ProcedurallySpawnObjects(BP_Pillar, PillarAmount, true);
	ProcedurallySpawnObjects(BP_MovableBlocks, MovableBlockAmount, true, 150);
	ProcedurallySpawnObjects(BP_Coin, CoinAmount, true, 50);
	ProcedurallySpawnObjects(BP_Gem, GemAmount, true, 100);
}

/**
 * destroy all the objects of the same type of the parameter in the scene
 * @param BP_Object The class of the object to destroy
 */
void AProceduralMap::CleanObject(const TSubclassOf<AActor> BP_Object) const
{
	TArray<AActor*> Objects;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_Object, Objects);
	for (AActor* Object : Objects)
	{
		Object->Destroy();
	}
}

/**
 * @brief Procedurally spawn objects in the floor, takes into account to not spawn objects inside the walls on the edge of the floor,
 * it tries to spawn the objects in a random location and rotation, if bCheckCollision is true it checks if the object overlaps with other objects,
 * if it does it destroys the object and tries again, if it fails to spawn the object after 100 attempts it stops trying.
 * If the object is an archway it rotate the objects only in 4 directions (0, 45, 90, 135) and place them farther from the edges of the floor avoiding clipping with the edge's walls.
 * @param ObjectClass The class of the object to spawn
 * @param NumberOfObjects The number of objects to spawn, defaults to 1
 * @param bCheckCollision Whether to check for collision with other objects, defaults to false
 * @param ZHeight The height at which to spawn the objects, defaults to 0
 */
void AProceduralMap::ProcedurallySpawnObjects(UClass* ObjectClass, const int32 NumberOfObjects, const bool bCheckCollision, const float ZHeight) const
{
	const float HalfFloorLength = FloorLength / 2; // to shorten the calculations
	const float HalfFloorWidth = FloorWidth / 2;
	constexpr int32 DistanceFromWalls = 100;  // min spawning distance from the walls on the edge of the floor in cm
	
	if (HalfFloorLength <= DistanceFromWalls || HalfFloorWidth <= DistanceFromWalls)
	{
		UE_LOG(LogTemp, Error, TEXT("The floor is too small to spawn objects!"));
		return;
	}
	
	int32 SpawnedObjects = 0;
	constexpr int32 MaxAttempts = 100;
	int32 Attempts = 0;
	while (SpawnedObjects < NumberOfObjects && Attempts < MaxAttempts)
	{
		FVector SpawnLocation;
		FRotator SpawnRotation;
		if (ObjectClass == BP_Archway)
		{
			int32 RandomSide = FMath::RandRange(0, 3);  // only 4 rotations
			if (RandomSide == 0) /* | */
			{
				SpawnRotation = FRotator(0, 0, 0);
				SpawnLocation = FVector(FMath::RandRange(-HalfFloorWidth + (4 * DistanceFromWalls), HalfFloorWidth - (4 * DistanceFromWalls)),
										FMath::RandRange(-HalfFloorLength + DistanceFromWalls, HalfFloorLength - DistanceFromWalls), ZHeight);
			}
			else if (RandomSide == 1) /* / */
			{
				SpawnRotation = FRotator(0, 45, 0);
				SpawnLocation = FVector(FMath::RandRange( -HalfFloorWidth + 3 * DistanceFromWalls, HalfFloorWidth - (3 * DistanceFromWalls)),
										FMath::RandRange(-HalfFloorLength + 3 * DistanceFromWalls, HalfFloorLength - (3 * DistanceFromWalls)), ZHeight);
			}
			else if (RandomSide == 2) /* - */
			{
				SpawnRotation = FRotator(0, 90, 0);
				SpawnLocation = FVector(FMath::RandRange(-HalfFloorWidth + DistanceFromWalls, HalfFloorWidth - DistanceFromWalls),
										FMath::RandRange(-HalfFloorLength + 4 * DistanceFromWalls, HalfFloorLength - (4 * DistanceFromWalls)), ZHeight);
			}
			else if (RandomSide == 3) /* \ */
			{
				SpawnRotation = FRotator(0, 135, 0);
				SpawnLocation = FVector(FMath::RandRange(-HalfFloorWidth + (3 * DistanceFromWalls), HalfFloorWidth - 3 * DistanceFromWalls),
										FMath::RandRange(-HalfFloorLength + 3 * DistanceFromWalls, HalfFloorLength - (3 * DistanceFromWalls)), ZHeight);
			}
		}
		else  // every other object
		{
			SpawnLocation = FVector(FMath::RandRange(-HalfFloorWidth + DistanceFromWalls, HalfFloorWidth - DistanceFromWalls),
									FMath::RandRange(-HalfFloorLength + DistanceFromWalls, HalfFloorLength - DistanceFromWalls), ZHeight);
			SpawnRotation = FRotator(0, FMath::RandRange(0.f, 360.f), 0);
		}
		if (IsActorCollidingWithPlayerStart(SpawnLocation))
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn location overlaps with Player Start!"));
			Attempts++;
			continue;
		}
		
		FActorSpawnParameters SpawnInfo;
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ObjectClass, SpawnLocation, SpawnRotation, SpawnInfo);
		if (SpawnedActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnedActor is null!"));
			Attempts++;
			continue;
		}
		// UE_LOG(LogTemp, Warning, TEXT("SpawnedActor: %s. Location: %s"), *SpawnedActor->GetName(), *SpawnedActor->GetActorLocation().ToString());
		
		if (bCheckCollision)
		{
			const UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(SpawnedActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			FVector BoxHalfExtent = MeshComponent->GetStaticMesh()->GetBoundingBox().GetExtent();
			if (IsActorColliding(SpawnedActor, SpawnLocation, BoxHalfExtent))
			{
				// UE_LOG(LogTemp, Warning, TEXT("BoxHalfExtent: %s"), *BoxHalfExtent.ToString());
				// UE_LOG(LogTemp, Warning, TEXT("Overlap detected!"));
				SpawnedActor->Destroy();
				Attempts++;
				continue;
			}
		}

		SpawnedObjects++;
	}
}

/**
 * Check if the ActorSpawnLocation overlaps with other objects given the ActorBoxHalfExtent, the dimensions of the object's bounding box.
 * @param ActorToCheck The actor to ignore when checking for overlap, usually the actor itself that is calling this function
 * @param ActorSpawnLocation The location at which theobject is going to be spawned
 * @param ActorBoxHalfExtent The half extent of the object's bounding box
 * @return TRUE Whether the object overlaps with other objects
 */
bool AProceduralMap::IsActorColliding(const AActor* ActorToCheck, const FVector& ActorSpawnLocation, const FVector& ActorBoxHalfExtent) const
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(ActorToCheck); // Ignore self if needed

	TArray<FOverlapResult> HitResults;
	const bool bHit = GetWorld()->OverlapMultiByObjectType(
		HitResults,
		ActorSpawnLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		FCollisionShape::MakeBox(ActorBoxHalfExtent), // Adjust the shape based on your object's size
		CollisionParams
	);

	return bHit;
}

bool AProceduralMap::IsActorCollidingWithPlayerStart(const FVector& ActorSpawnLocation)
{
	// 200 is 2m, within 2m from the player start, which is (0, 0, 0) in the world, we don't want to spawn objects
	if (ActorSpawnLocation.X > -200 && ActorSpawnLocation.X < 200 && ActorSpawnLocation.Y > -200 && ActorSpawnLocation.Y < 200)
	{
		return true;
	}
	return false;
}
