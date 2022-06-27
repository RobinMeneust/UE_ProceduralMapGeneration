// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall.h"
#include "MapGen.generated.h"

typedef struct RoomType {
	int min_width;
	int max_width;
	int min_quantity;
	int max_quantity;
	int current_quantity;
}RoomType;

typedef struct Map {
	int x_width;
	int y_width;
	int ** grid;
}Map;

typedef struct Room {
	TArray<AWall*> walls;
	int type; /* 0 : corridor | 1 : basic | ...*/
	int index;
}Room;

typedef struct Path {
	FIntPoint pos;
	bool walls[4]; // 0: left, 1: top, 2: right, 3: bot
	TArray<AWall*> wallElements;
}Path;

UCLASS()
class TESTSC_API AMapGen : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapGen();
	AWall* addWall(FVector start, FVector end, bool isDoor = false, double height = 2.5);
	void generateMesh();
	Room addRoom(FIntPoint start, FIntPoint end, int type);
	void buildRooms();
	FIntPoint getRandomPoint(int x_min, int x_max, int y_min, int y_max);
	bool isAFreeSpace(FIntPoint coordStart, FIntPoint coordEnd);
	bool areAlreadyConnected(FIntPoint items, TArray<FIntPoint> connections);
	void buildDoors();
	void initializePath(Path* path);
	int getValidNeighbour(FIntPoint cell, FIntPoint* newCell);
	void buildCorridors();
	void insertDoor(FVector newDoorCoord[2], int roomTested);

	Map m_map; // Contains all the rooms index, if m_map.grid[i][j] == -1 then it's empty, there is no room created here
	TArray<Room> m_rooms;
	TArray<Path> m_corridors;
	RoomType m_roomTypes[2];
	int m_numberOfRoomTypes = 2;
	Room m_borders;
	double m_wallThickness = 15.0;
	double m_shift = 7.5;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	int m_nbOfRooms = -1;
};
