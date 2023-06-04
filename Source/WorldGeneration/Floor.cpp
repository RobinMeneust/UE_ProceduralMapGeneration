// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFloor::AFloor()
{
	// in centimeters
	thickness = 10.0; 
	start = FVector(0,0,0);
	end = FVector(100,100,0);
}

void AFloor::createFloor() {
	floorObject = GetWorld()->SpawnActor<AWall>(AWall::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	/*
	thickness wall = width x floor
	height wall = thickness floor
	length wall = width y floor
	*/
	floorObject->m_height = thickness;
	floorObject->m_start = FVector((end.X + start.X) / 2, start.Y, end.Z);
	floorObject->m_end = FVector((end.X + start.X) / 2, end.Y, end.Z);
	floorObject->m_thickness = end.X - start.X;
	floorObject->m_length = FVector::Distance(floorObject->m_start, floorObject->m_end);

	floorObject->SetFolderPath(TEXT("Floor"));

	floorObject->createWall();
}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}