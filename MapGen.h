// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall.h"
#include "MapGen.generated.h"

typedef struct Room {
	TArray<AWall*> walls;
	int type; // not used yet
	int index;
}Room;

UCLASS()
class TESTSC_API AMapGen : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapGen();
	void buildMapBorders();
	AWall* addWall(FVector start, FVector end);
	Room addRoom(FVector start, FVector end);

	TArray<Room> m_rooms;
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
	int m_nbOfRooms = 0;
};
