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

		UE_LOG(LogTemp, Log, TEXT("newRoomCoord = %f %f | otherRoomsCoord %f %f"), coord.X, coord.Y, m_roomsList[i].object->GetActorLocation().X, m_roomsList[i].object->GetActorLocation().Y);
		UE_LOG(LogTemp, Log, TEXT("D = %f"), FVector::Distance(coord, m_roomsList[i].object->GetActorLocation()));
		if (FVector::Distance(coord, m_roomsList[i].object->GetActorLocation())<m_max_roomWidth) {
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
	int index = 0;
	FVector roomScale = { 1.0 ,1.0 ,1.0 };
	for (int i = 0; i < m_maxNbOfRooms; i++) {
		FVector coord = FVector::ZeroVector;
		do {
			coord = getRandomPointWithinRadius() + GetActorLocation();
			coord.Z = 50.0f; // only for testing, so that it doesn't overlap with the rest of the scene
			iter++;
		} while (isOverlapping(coord) && iter < 10);
		if(iter<10){ //the last coordinates don't overlap with the other rooms so we can create a new room here
			AActor* newSpawnedRoomObject = GetWorld()->SpawnActor<AActor>(m_roomToSpawn, coord, FRotator::ZeroRotator);
			Room newSpawnedRoom;
			roomScale.X = FMath::RandRange(0.0f, 1.0f) * (m_max_roomWidth - m_min_roomWidth) + m_min_roomWidth;
			roomScale.Y = FMath::RandRange(0.0f, 1.0f) * (m_max_roomWidth - m_min_roomWidth) + m_min_roomWidth;
			roomScale.Z = 1.0f;
			newSpawnedRoomObject->SetActorScale3D(roomScale);
			
			newSpawnedRoom.object = newSpawnedRoomObject;
			newSpawnedRoom.index = index;
			m_roomsList.Add(newSpawnedRoom);
			UE_LOG(LogTemp, Log, TEXT("IN generate Rooms %d"), i);
			index++;
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

