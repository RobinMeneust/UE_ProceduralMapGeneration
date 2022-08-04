// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Wall.generated.h"

UCLASS()
class TESTSC_API AWall : public AActor
{
	GENERATED_BODY()

	TArray<FVector> m_vertices;
	TArray<int32> m_triangles;
	TArray<FVector2D> m_uvs;
	int m_verticesIndex;

	UProceduralMeshComponent* m_mesh;

public:
	FVector m_start = { 0.0, 0.0, 0.0 };
	FVector m_end = { 10.0, 0.0, 0.0 };
	double m_length = 15.0;
	double m_thickness = 15.0;
	double m_shift = m_thickness/2;
	double m_height = 200.0;
	bool m_isEmpty = false;

	void init();
	void createMesh();
	void createSquareMesh(int i_face);
	void placeWall();
	double getAngleBetweenVectors(FVector u, FVector v);
	void createWall();
	void setMaterial(UMaterial* material);
	
private:
	FVector m_refVertices[8]{
		{ 0.0, 0.0, 0.0 }, // Bottom Left Front
		{ 0.0, 0.0, 200.0 }, // Top Left Front
		{ 15.0, 0.0, 0.0 }, // Bottom Right Front
		{ 15.0, 0.0, 200.0 }, // Top Right Front

		{ 0.0, 15.0, 0.0 }, // Bottom Left Back
		{ 0.0, 15.0, 200.0 }, // Top Left Back
		{ 15.0, 15.0, 0.0 }, // Bottom Right Back
		{ 15.0, 15.0, 200.0 } // Top Right Back
	};

	int m_refTriangles[6][4]{
		{ 0, 1, 2, 3 }, // FRONT
		{ 6, 7, 4, 5 }, // BACK
		{ 4, 5, 0, 1 }, // LEFT
		{ 2, 3, 6, 7 }, // RIGHT
		{ 4, 0, 6, 2 }, // TOP
		{ 1, 5, 3, 7 } // BOT
	};
	
public:	
	// Sets default values for this actor's properties
	AWall();

protected:
	UMaterial* m_material;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
