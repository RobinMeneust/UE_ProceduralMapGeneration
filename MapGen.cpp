// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGen.h"



AWall* AMapGen::addWall(FVector start, FVector end) {

	AWall* newWall;
	newWall = GetWorld()->SpawnActor<AWall>(AWall::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	newWall->m_start = start;
	newWall->m_end = end;
	newWall->createWall();

	return newWall;
}

Room AMapGen::addRoom(FVector coordStart, FVector coordEnd, int type) {
	Room newRoom;
	newRoom.index = m_nbOfRooms;
	newRoom.coord_start = coordStart;
	newRoom.coord_end = coordEnd;
	m_nbOfRooms++;
	if(type>=0)
		m_roomTypes[type].current_quantity++;
	return newRoom;
}

// the function underneath has to be improved
void AMapGen::generateMesh() {
	// We readjust the vectors with m_shift so that the walls have "valid" edges (we have 1 edge and not 2 at an intersection between 2 walls)

	// We build the map borders
	FVector coordStart(m_borders.coord_start), coordEnd(m_borders.coord_end);

	m_borders.walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));
	m_borders.walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));
	m_borders.walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y - m_shift, coordStart.Z }));
	m_borders.walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));

	// We build all the room walls
	for (int i = 0; i < m_nbOfRooms; i++) {
		coordStart = m_rooms[i].coord_start;
		coordEnd = m_rooms[i].coord_end;
		if (m_rooms[i].doorsArray.Num()>0) { // if we have a door
			for (int j = 0; j < m_rooms[i].doorsArray.Num(); j++) {
				switch (m_rooms[i].doorsArray[j].face) {
					case 0: // left
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, m_rooms[i].doorsArray[j].coordStart + m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordStart.X, m_rooms[i].doorsArray[j].coordEnd - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));

						m_rooms[i].walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));
						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y - m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));
						break;
					case 1: // right
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));

						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, m_rooms[i].doorsArray[j].coordStart - m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordEnd.X, m_rooms[i].doorsArray[j].coordEnd + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y, coordEnd.Z }));

						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));
						break;
					case 2: // bot
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));
						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y - m_shift, coordStart.Z }));

						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { m_rooms[i].doorsArray[j].coordStart, coordStart.Y, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ m_rooms[i].doorsArray[j].coordEnd, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));
						break;
					case 3: // top
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));

						m_rooms[i].walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { m_rooms[i].doorsArray[j].coordStart , coordEnd.Y, coordEnd.Z }));
						m_rooms[i].walls.Add(addWall({ m_rooms[i].doorsArray[j].coordEnd, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));

						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y - m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));
						break;
				}
			}
		}
		else {
			m_rooms[i].walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));
			m_rooms[i].walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));
			m_rooms[i].walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y - m_shift, coordStart.Z }));
			m_rooms[i].walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));
		}
	}
}

AMapGen::AMapGen()
{
	m_borders.index = -1;
	m_borders.coord_start = { 0.0, 0.0, 0.0 };
	m_borders.coord_end = { 5000.0, 5000.0, 0.0 };
	//CORRIDOR
	m_roomTypes[0].min_width = 100.0; // 1m
	m_roomTypes[0].min_quantity = 5;
	m_roomTypes[0].current_quantity = 0;

	//BASIC ROOM
	m_roomTypes[1].min_width = 300.0; // 3m
	m_roomTypes[1].max_width = 1500.0;
	m_roomTypes[1].min_quantity = 8;
	m_roomTypes[1].proportion = 0.5;
	m_roomTypes[1].current_quantity = 0;
}

bool AMapGen::isAFreeSpace(FVector coordStart, FVector coordEnd) {
	// IS OUT OF THE MAP
	if (coordStart.X < m_borders.coord_start.X ||
		coordEnd.X > m_borders.coord_end.X ||
		coordStart.Y < m_borders.coord_start.Y ||
		coordEnd.Y > m_borders.coord_end.Y)
	{
		UE_LOG(LogTemp, Log, TEXT("is out of the map | S(%lf %lf %lf) | E(%lf %lf %lf)"), coordStart.X, coordStart.Y, coordStart.Z, coordEnd.X, coordEnd.Y, coordEnd.Z);
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("is not out of the map"));

	// IS OVERLAPPING WITH OTHER ROOMS
	for (int i = 0; i < m_nbOfRooms; i++) {
		if (coordStart.X < m_rooms[i].coord_end.X &&
			coordEnd.X > m_rooms[i].coord_start.X &&
			coordStart.Y < m_rooms[i].coord_end.Y &&
			coordEnd.Y > m_rooms[i].coord_start.Y) 
		{
			return false;
		}
	}
	return true;
}

Door AMapGen::getDoorData(int face, FVector parentRoomCoordStart, FVector parentRoomCoordEnd, FVector newRoomCoordStart, FVector newRoomCoordEnd)
{
	Door newDoor;
	newDoor.face = face;

	// start -> end : clockwise
	switch (face) {
	case 0: // left
		newDoor.coordStart = parentRoomCoordEnd.Y;
		newDoor.coordEnd = newRoomCoordStart.Y;
		break;
	case 1: // right
		newDoor.coordStart = parentRoomCoordStart.Y;
		newDoor.coordEnd = newRoomCoordEnd.Y;
		break;
	case 2: // bot
		newDoor.coordStart = parentRoomCoordStart.X;
		newDoor.coordEnd = newRoomCoordEnd.X;
		break;
	case 3: // top
		newDoor.coordStart = parentRoomCoordEnd.X;
		newDoor.coordEnd = newRoomCoordStart.X;
		break;
	}
	return newDoor;
}

void AMapGen::buildRooms()
{
	int iter = 0;
	int iter_search = 0;
	bool continueConstruction = true;
	bool continueSearch = true;
	double shiftXRangeStart(0.0), shiftXRangeEnd(0.0), shiftYRangeStart(0.0), shiftYRangeEnd(0.0);
	double shift = 0;
	//FIRST ROOM
	m_rooms.Add(addRoom({ 0.0, 0.0, 0.0 }, {500.0, 500.0, 0.0}, 1));

	do {
		continueSearch = true;
		continueConstruction = true;

		FVector newRoomCoordStart = FVector::ZeroVector;
		FVector newRoomCoordEnd = { 500.0, 500.0, 0.0 }; // Default value : 5x5x5
		FVector newRoomScale = { FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width), FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width), 2.0 };

		// We want to have at least 1m to place a door so if the room is too small we can't move it too much from its default position hence we add this test
		if (newRoomScale.X > 400.0) {
			shiftXRangeStart = -3 * newRoomScale.X / 8;
			shiftXRangeEnd = 3 * newRoomScale.X / 8;
		}
		if (newRoomScale.Y > 400.0) {
			shiftYRangeStart = -3 * newRoomScale.Y / 8;
			shiftYRangeEnd = 3 * newRoomScale.Y / 8;
		}
		

		do{
			for (int i = m_nbOfRooms - 1; i >= 0; i--) {
				for (int j = 0; j < 4; j++) { // faces of the previous room that we are checking
					newRoomCoordStart = m_rooms[i].coord_start;
					switch (j) {
						case 0: newRoomCoordStart.X -= newRoomScale.X; break; // x<0
						case 1: newRoomCoordStart.X = m_rooms[i].coord_end.X; break; // x>0
						case 2: newRoomCoordStart.Y -= newRoomScale.Y; break; // y<0
						case 3: newRoomCoordStart.Y = m_rooms[i].coord_end.Y; break; // y>0
					}
					// We move the new room randomly but it needs to share a wall with m_rooms[i]
					
					if (j < 2) {
						newRoomCoordStart.Y += FMath::RandRange(shiftYRangeStart, shiftYRangeEnd);
					}
					else {
						newRoomCoordStart.X += FMath::RandRange(shiftXRangeStart, shiftXRangeEnd);
					}
					
					newRoomCoordEnd = newRoomCoordStart + newRoomScale;
					if (isAFreeSpace(newRoomCoordStart, newRoomCoordEnd)) {
						continueSearch = false;
						break;
					}
				}
				if (!continueSearch)
					break;
			}
			iter_search++;
		} while (continueSearch && iter_search<100);
		iter_search = 0;

		m_rooms.Add(addRoom(newRoomCoordStart, newRoomCoordEnd, 1));
		

		//m_rooms.Add ()... when we will have more types of room

		//We check if all the rooms have been added
		for (int i = 0; i < m_numberOfRoomTypes; i++) {
			if (m_roomTypes[i].current_quantity >= m_roomTypes[i].min_quantity) {
				continueConstruction = false;
				UE_LOG(LogTemp, Log, TEXT("STOP CONSTR : i = %d | current = %d | limit = %d"), i, m_roomTypes[i].current_quantity, m_roomTypes[i].min_quantity);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("iter = %d | continueSearch= %d | continue construction %d"), iter, continueSearch, continueConstruction);
		iter++;
	} while (iter<1000 && continueConstruction); // iter will be increased when the map gets bigger
}

void AMapGen::buildDoors() {

}

// Called when the game starts or when spawned
void AMapGen::BeginPlay()
{
	Super::BeginPlay();	
	m_borders = addRoom(m_borders.coord_start, m_borders.coord_end, -1);
	buildRooms();
	//buildCorridors();
	//buildDoors();
	generateMesh();
	
}

// Called every frame
void AMapGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

