// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall.h"
#include "MapGen.generated.h"

typedef struct RoomType {
	double min_width;
	double max_width;
	int min_quantity;
	int current_quantity;
	double proportion;
}RoomType;


typedef struct Door {
	int face;
	double coordStart;
	double coordEnd;
}Door;

typedef struct Room {
	TArray<AWall*> walls;
	int type; /* 0 : corridor | 1 : basic | ...*/
	int index;
	FVector coord_start;
	FVector coord_end;
	TArray<Door> doorsArray;
}Room;

UCLASS()
class TESTSC_API AMapGen : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapGen();
	AWall* addWall(FVector start, FVector end);
	void generateMesh();
	Room addRoom(FVector coordStart, FVector coordEnd, int type);
	void buildRooms();
	void buildDoors();
	bool isAFreeSpace(FVector coordStart, FVector coordEnd);
	Door getDoorData(int face, FVector parentRoomCoordStart, FVector parentRoomCoordEnd, FVector newRoomCoordStart, FVector newRoomCoordEnd);

	TArray<Room> m_rooms;
	RoomType m_roomTypes[2];
	int m_numberOfRoomTypes = 2;
	Room m_borders;
	double m_wallThickness = 15.0;
	double m_shift = 7.5;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	int m_nbOfRooms = -1; // -1 corresponds to the map borders
};
