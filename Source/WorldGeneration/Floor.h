// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall.h"
#include "Floor.generated.h"

UCLASS()
class WORLDGENERATION_API AFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWall* floorObject;

	// Sets default values for this actor's properties
	AFloor();
	void createFloor();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float thickness; // in centimeters
	FVector start;
	FVector end;

};
