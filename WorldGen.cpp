// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"
#include "Room.h"

// Sets default values
AWorldGen::AWorldGen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

FVector AWorldGen::getRandomPointWithinRadius()
{
	FVector coord = FVector::ZeroVector;
	float r = FMath::RandRange(0.0f, 1.0f) * m_radius;
	float theta = FMath::RandRange(0.0f, 1.0f) * 2*PI;

	// Change of coordinate system
	coord.X = r * cos(theta);
	coord.Y = r * sin(theta);

	return coord;
}

bool AWorldGen::isOverlapping(FVector coord)
{
	UE_LOG(LogTemp, Log, TEXT("BEGIN"));
	for (int i = 0; i < m_roomsList.Num(); i++) {
		//FOR DEBUG

		UE_LOG(LogTemp, Log, TEXT("newRoomCoord = %f %f | otherRoomsCoord %f %f | D = %f"), coord.X, coord.Y, m_roomsList[i]->GetActorLocation().X, m_roomsList[i]->GetActorLocation().Y);
		//UE_LOG(LogTemp, Log, TEXT("D = %f"), FVector::Distance(coord, m_roomsList[i]->GetActorLocation()));
		if (FVector::Distance(coord, m_roomsList[i]->GetActorLocation())<10) {
			UE_LOG(LogTemp, Log, TEXT("END_TRUE"));
			return true;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("END_FALSE"));
	return false;
}

void AWorldGen::generateRooms()
{
	int iter = 0; // nb of iterations before finding an empty space to palce a room
	for (int i = 0; i < m_maxNbOfRooms; i++) {
		FVector coord = FVector::ZeroVector;
		do {
			coord = getRandomPointWithinRadius() + GetActorLocation();
			coord.Z = 50.0f; // only for testing, so that it doesn't overlap with the rest of the scene
			iter++;
		} while (isOverlapping(coord) && iter < 10);
		if(iter>=10){//it overlaps
			AActor* newSpawnedRoom = GetWorld()->SpawnActor<AActor>(m_roomToSpawn, coord, FRotator::ZeroRotator);
			m_roomsList.Add(newSpawnedRoom);
		}
		iter = 0;
	}
}

// Called when the game starts or when spawned
void AWorldGen::BeginPlay()
{
	Super::BeginPlay();
	generateRooms();
}

// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

