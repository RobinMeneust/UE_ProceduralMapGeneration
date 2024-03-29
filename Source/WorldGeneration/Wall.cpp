// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AWall::AWall()
{
	m_mesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProcMesh");
	RootComponent = m_mesh;
	m_material = LoadObject<UMaterial>(nullptr, TEXT("/Game/StarterContent/Materials/M_Brick_Clay_New"));
}

void AWall::createSquareMesh(int i_face) {
	float max = 1;
	//top-left -> bot-left -> top-right -> bot-right
	/*
	front : 0 1 2 3
	back : 6 7 4 5
	left : 4 5 0 1
	right : 2 3 6 7
	top : 4 0 6 2
	bot : 1 5 3 7
	*/
	for(int i=0; i<4; i++)
		m_vertices.Add(m_refVertices[m_refTriangles[i_face][i]]);

	if (i_face == 0 || i_face == 1) { // front & back
		m_uvs.Add(FVector2D(0, 0));
		m_uvs.Add(FVector2D(0, m_height / 100));
		m_uvs.Add(FVector2D(m_thickness/100, 0));
		m_uvs.Add(FVector2D(m_thickness / 100, m_height / 100));
	}
	else if (i_face == 2 || i_face == 3) { // left & right
		m_uvs.Add(FVector2D(0, 0));
		m_uvs.Add(FVector2D(0, m_height / 100));
		m_uvs.Add(FVector2D(m_length / 100, 0));
		m_uvs.Add(FVector2D(m_length / 100, m_height / 100));
	}
	else if (i_face == 4 || i_face == 5) { // top & bot
		m_uvs.Add(FVector2D(0, 0));
		m_uvs.Add(FVector2D(0, m_length / 100));
		m_uvs.Add(FVector2D(m_thickness / 100, 0));
		m_uvs.Add(FVector2D(m_thickness / 100, m_length / 100));
	}

	//Triangle1
	m_triangles.Add(m_verticesIndex);
	m_triangles.Add(m_verticesIndex + 1);
	m_triangles.Add(m_verticesIndex + 2);

	//Triangle2
	m_triangles.Add(m_verticesIndex + 2);
	m_triangles.Add(m_verticesIndex + 1);
	m_triangles.Add(m_verticesIndex + 3);

	m_verticesIndex += 4;
}

void AWall::createMesh() {
	m_verticesIndex = 0;
	for (int i = 0; i < 6; i++)
		createSquareMesh(i);
	
	m_mesh->CreateMeshSection(0, m_vertices, m_triangles, TArray<FVector>(), m_uvs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

void AWall::init() {
	if (m_start.X > m_end.X) { // if the start isn't on the "front" side then we switch it with the end
		double temp=m_start.X;
		m_start.X = m_end.X;
		m_end.X = temp;
	}
	m_length = FVector::Distance(m_end, m_start);
	m_shift = m_thickness / 2;

	float max = (m_height > m_length) ? m_height : m_length;

	//FRONT
	m_refVertices[0] = { -m_shift, 0.0, 0.0 }; // Bottom Left Front
	m_refVertices[1] = { -m_shift, 0.0, m_height }; // Top Left Front
	m_refVertices[2] = { m_shift, 0.0, 0.0 }; // Bottom Right Front
	m_refVertices[3] = { m_shift, 0.0, m_height }; // Top Right Front

	//BACK
	m_refVertices[4] = { -m_shift, m_length, 0.0 }; // Bottom Left Front
	m_refVertices[5] = { -m_shift, m_length, m_height }; // Top Left Front
	m_refVertices[6] = { m_shift, m_length, 0.0 }; // Bottom Right Front
	m_refVertices[7] = { m_shift, m_length, m_height }; // Top Right Front
}

double AWall::getAngleBetweenVectors(FVector u, FVector v) {
	// We ignore the Z axis and we normalize u and v
	u.Z = 0;
	v.Z = 0;
	u.Normalize(0);
	v.Normalize(0);

	
	//UE_LOG(LogTemp, Log, TEXT("U = (%lf;%lf) | V = (%lf;%lf) | DOT_PRODUCT = %lf"), u.X, u.Y, v.X, v.Y, FVector::DotProduct(u, v));
	if(FVector::CrossProduct(u,v).Z>0)
		return UKismetMathLibrary::Acos(FVector::DotProduct(u, v)) * (180 / PI); // in degrees;
	else
		return -1.0 * UKismetMathLibrary::Acos(FVector::DotProduct(u, v)) * (180 / PI); // in degrees;
}

void AWall::placeWall() {
	FVector newLocation = GetActorLocation();
	FRotator newRotation = GetActorRotation();

	FVector currentDirection = { 0.0, m_length, 0.0 };
	FVector newDirection = { (m_end - m_start).X, (m_end - m_start).Y , 0.0};

	float deltaRotation = getAngleBetweenVectors(currentDirection, newDirection);
	newRotation.Yaw += deltaRotation;
	newLocation += m_start;
	SetActorLocationAndRotation(newLocation, newRotation);
}

void AWall::createWall() {
	if (!m_isEmpty) {// if it's not an empty space (like a door going from the ground to the ceiling)
		init();
		createMesh();
		setMaterial(m_material);
		placeWall();
	}
}

void AWall::setMaterial(UMaterial* material) {
	if (material)
	{
		m_mesh->SetMaterial(0, material);
	}
}


// Called when the game starts or when spawned
void AWall::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

