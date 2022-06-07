// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGen.generated.h"

UCLASS()
class TESTSC_API AWorldGen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGen();
	FVector getRandomPointWithinRadius();
	void generateRooms();
	bool isOverlapping(FVector coord);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen")
	int m_maxNbOfRooms = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen")
	float m_radius = 1000.0f; // in cm
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen")
	UClass* m_roomToSpawn = NULL;

	TArray<AActor*> m_roomsList;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
