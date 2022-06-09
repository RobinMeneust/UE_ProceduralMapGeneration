// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGen.h"



AWall* AMapGen::addWall(FVector start, FVector end) {

	AWall* newWall;
	newWall = GetWorld()->SpawnActor<AWall>(AWall::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	newWall->m_start = start;
	newWall->m_end = end;
	newWall->createWall();

	return newWall;
}

void AMapGen::buildMapBorders() {
	m_borders.walls.Add(addWall({ 0.0, 0.0, 0.0 }, { 5000.0, 0.0, 0.0 }));
	m_borders.walls.Add(addWall({ 5000.0, 0.0, 0.0 }, { 5000.0, 5000.0, 0.0 }));
	m_borders.walls.Add(addWall({ 5000.0, 5000.0, 0.0 }, { 0.0, 5000.0, 0.0 }));
	m_borders.walls.Add(addWall({ 0.0, 5000.0, 0.0 }, { 0.0, 0.0, 0.0 }));
}


Room AMapGen::addRoom(FVector start, FVector end) {
	Room newRoom;
	newRoom.index = m_nbOfRooms;
	m_nbOfRooms++;

	// We readjust the vectors so that the walls have "valid" edges (we have 1 edge and not 2 at an intersection between 2 walls)

	newRoom.walls.Add(addWall({ start.X, start.Y - m_shift, start.Z }, { start.X, end.Y + m_shift, start.Z }));
	newRoom.walls.Add(addWall({ start.X, end.Y, start.Z }, { end.X, end.Y, end.Z }));
	newRoom.walls.Add(addWall({ end.X, end.Y + m_shift, end.Z }, { end.X, start.Y - m_shift, start.Z }));
	newRoom.walls.Add(addWall({ end.X, start.Y, start.Z }, { start.X, start.Y, start.Z }));
	
	return newRoom;
}

// Sets default values
AMapGen::AMapGen()
{
	m_borders.index = -1;
}

// Called when the game starts or when spawned
void AMapGen::BeginPlay()
{
	Super::BeginPlay();	
	buildMapBorders();
	m_rooms.Add(addRoom({ 0.0, 0.0, 0.0 }, {500.0, 500.0, 0.0 }));
	
}

// Called every frame
void AMapGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

