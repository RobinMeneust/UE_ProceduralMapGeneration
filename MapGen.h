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

typedef struct Door {
	int face; // left -> right -> bot -> top
	int coordStart;
	int coordEnd;
}Door;

typedef struct Room {
	TArray<AWall*> walls;
	int type; /* 0 : corridor | 1 : basic | ...*/
	int index;
	FIntVector coord_start;
	FIntVector coord_end;
	TArray<Door> doorsArray;
}Room;

UCLASS()
class TESTSC_API AMapGen : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapGen();
	AWall* addWall(FVector start, FVector end, double height = 2.5);
	void generateMesh();
	Room addRoom(FIntVector coordStart, FIntVector coordEnd, int type);
	void buildRooms();
	void buildDoors();
	bool isAFreeSpace(FIntVector coordStart, FIntVector coordEnd);
	Door getDoorData(int face, FVector parentRoomCoordStart, FVector parentRoomCoordEnd, FVector newRoomCoordStart, FVector newRoomCoordEnd);
	bool areAlreadyConnected(FIntPoint items, TArray<FIntPoint> connections);

	Map m_map; // Contains all the rooms index, if m_map.grid[i][j] == -1 then it's empty, there is no room created here
	TArray<Room> m_rooms;
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
	int m_nbOfRooms = -1; // -1 corresponds to the map borders
};
