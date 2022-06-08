// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AWall::AWall()
{
	m_mesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProcMesh");
	RootComponent = m_mesh;

}

void AWall::createSquareMesh(int i_face) {
	//top-left -> bot-left -> top-right -> bot-right
	/*
	front : 0 1 2 3
	back : 6 7 4 5
	left : 4 5 0 1
	right : 2 3 6 7
	top : 4 0 6 2
	bot : 1 5 3 7
	*/

	m_uvs.Add(FVector2D(0, 0));
	m_uvs.Add(FVector2D(0, 1));
	m_uvs.Add(FVector2D(1, 0));
	m_uvs.Add(FVector2D(1, 1));

	//Triangle1
	m_triangles.Add(m_refTriangles[i_face][0]);
	m_triangles.Add(m_refTriangles[i_face][1]);
	m_triangles.Add(m_refTriangles[i_face][2]);

	//Triangle2
	m_triangles.Add(m_refTriangles[i_face][2]);
	m_triangles.Add(m_refTriangles[i_face][1]);
	m_triangles.Add(m_refTriangles[i_face][3]);

	if (m_material)
	{
		m_mesh->SetMaterial(0, m_material);
	}
}

void AWall::createMesh() {
	for(int i=0; i<8; i++)
		m_vertices.Add(m_refVertices[i]);

	for (int i_face = 0; i_face < 6; i_face++) {
		createSquareMesh(i_face);
	}
	m_mesh->CreateMeshSection(0, m_vertices, m_triangles, TArray<FVector>(), m_uvs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

void AWall::initializeVertices() {
	/*
		{ 0.0, 0.0, 0.0 }, // Bottom Left Front
		{ 0.0, 0.0, 200.0 }, // Top Left Front
		{ 15.0, 0.0, 0.0 }, // Bottom Right Front
		{ 15.0, 0.0, 200.0 }, // Top Right Front

		{ 0.0, 15.0, 0.0 }, // Bottom Left Back
		{ 0.0, 15.0, 200.0 }, // Top Left Back
		{ 15.0, 15.0, 0.0 }, // Bottom Right Back
		{ 15.0, 15.0, 200.0 } // Top Right Back
	*/
	if (m_start.X > m_end.X) { // if the start isn't on the "front" side then we switch it with the end
		double temp=m_start.X;
	}
	m_length = FVector::Distance(m_end, m_start);
	
	//FRONT
	m_refVertices[0] = { 0.0, 0.0, 0.0 }; // Bottom Left Front
	m_refVertices[1] = { 0.0, 0.0, m_height }; // Top Left Front
	m_refVertices[2] = { m_thickness, 0.0, 0.0 }; // Bottom Right Front
	m_refVertices[3] = { m_thickness, 0.0, m_height }; // Top Right Front

	//BACK
	m_refVertices[4] = { 0.0, m_length, 0.0 }; // Bottom Left Front
	m_refVertices[5] = { 0.0, m_length, m_height }; // Top Left Front
	m_refVertices[6] = { m_thickness, m_length, 0.0 }; // Bottom Right Front
	m_refVertices[7] = { m_thickness, m_length, m_height }; // Top Right Front
}

void AWall::placeWall() {
	FVector newLocation = GetActorLocation();
	FRotator newRotation = GetActorRotation();

	//float DeltaRotation = ;
	//NewRotation.Yaw += DeltaRotation;
	//newRotation = UKismetMathLibrary::FindLookAtRotation(m_start, m_end);
	newLocation += m_start;
	//UE_LOG(LogTemp, Log, TEXT("Value = %f"), NewLocation.Z);
	SetActorLocationAndRotation(newLocation, newRotation);
}

// Called when the game starts or when spawned
void AWall::BeginPlay()
{
	Super::BeginPlay();
	initializeVertices();
	createMesh();
	placeWall();
}

// Called every frame
void AWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

