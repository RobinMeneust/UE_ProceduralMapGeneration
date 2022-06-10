// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGen.h"
#include "GenericPlatform/GenericPlatformMath.h"



AWall* AMapGen::addWall(FVector start, FVector end, double height) {

	AWall* newWall;
	newWall = GetWorld()->SpawnActor<AWall>(AWall::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	newWall->m_height = height * 100; // m to cm
	newWall->m_start = start;
	newWall->m_end = end;
	newWall->createWall();

	return newWall;
}

Room AMapGen::addRoom(FIntVector coordStart, FIntVector coordEnd, int type) {
	Room newRoom;
	newRoom.index = m_nbOfRooms;
	newRoom.coord_start = coordStart;
	newRoom.coord_end = coordEnd;
	for (int x = coordStart.X; x < coordEnd.X; x++) {
		for (int y = coordStart.Y; y < coordEnd.Y; y++) {
			m_map.grid[x][y] = newRoom.index;
		}
	}

	m_nbOfRooms++;
	if(type>=0)
		m_roomTypes[type].current_quantity++;
	return newRoom;
}

// the function underneath has to be improved
void AMapGen::generateMesh() {
	// We readjust the vectors with m_shift so that the walls have "valid" edges (we have 1 edge and not 2 at an intersection between 2 walls)

	// We build the map borders
	FVector coordStart(m_borders.coord_start*100), coordEnd(m_borders.coord_end*100); // *100 because we want cm and not meters

	m_borders.walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));
	m_borders.walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));
	m_borders.walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y - m_shift, coordStart.Z }));
	m_borders.walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));

	// We build all the room walls
	for (int i = 0; i < m_nbOfRooms; i++) {
		coordStart = FVector(m_rooms[i].coord_start*100);
		coordEnd = FVector(m_rooms[i].coord_end*100);

		if (m_rooms[i].doorsArray.Num()>0) { // if we have a door
			for (int j = 0; j < m_rooms[i].doorsArray.Num(); j++) {
				double doorStart = (double) m_rooms[i].doorsArray[j].coordStart;
				double doorEnd = (double)m_rooms[i].doorsArray[j].coordEnd;
				switch (m_rooms[i].doorsArray[j].face) {
					case 0: // left
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X,(double) doorStart + m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordStart.X, doorEnd - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));

						m_rooms[i].walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));
						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y - m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));
						break;
					case 1: // right
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));

						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, doorStart - m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordEnd.X, doorEnd + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y, coordEnd.Z }));

						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));
						break;
					case 2: // bot
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));
						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y - m_shift, coordStart.Z }));

						m_rooms[i].walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { doorStart, coordStart.Y, coordStart.Z }));
						m_rooms[i].walls.Add(addWall({ doorEnd, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));
						break;
					case 3: // top
						m_rooms[i].walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));

						m_rooms[i].walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { doorStart , coordEnd.Y, coordEnd.Z }));
						m_rooms[i].walls.Add(addWall({ doorEnd, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));

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

bool AMapGen::isAFreeSpace(FIntVector coordStart, FIntVector coordEnd) {
	// IS OUT OF THE MAP
	if (coordStart.X < m_borders.coord_start.X ||
		coordEnd.X > m_borders.coord_end.X ||
		coordStart.Y < m_borders.coord_start.Y ||
		coordEnd.Y > m_borders.coord_end.Y)
	{
		UE_LOG(LogTemp, Log, TEXT("is out of the map | S(%d %d %d) | E(%d %d %d)"), coordStart.X, coordStart.Y, coordStart.Z, coordEnd.X, coordEnd.Y, coordEnd.Z);
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("is not out of the map | S(%d %d %d) | E(%d %d %d)"), coordStart.X, coordStart.Y, coordStart.Z, coordEnd.X, coordEnd.Y, coordEnd.Z);

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
	int shiftXRangeStart(0), shiftXRangeEnd(0), shiftYRangeStart(0), shiftYRangeEnd(0);
	double shift = 0;
	FIntVector newRoomCoordStart = { 0, 0, 0 };
	FIntVector newRoomCoordEnd = { FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width), FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width), 0 };
	FIntVector newRoomScale;
	int i_face = 0;

	//FIRST ROOM
	m_rooms.Add(addRoom(newRoomCoordStart, newRoomCoordEnd, 1));

	do {
		continueSearch = true;
		continueConstruction = true;
		newRoomCoordStart = FIntVector::ZeroValue; // Default value : origin
		newRoomCoordEnd.X = 5; newRoomCoordEnd.Y = 5; newRoomCoordEnd.Z = 0; // Default value : 5x5 meters (at ground level)
		newRoomScale = { FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width), FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width), 0 };

		// We want to have at least 1m to place a door so if the room is too small we can't move it too much from its default position hence we add this test
		if (newRoomScale.X > 4) {
			shiftXRangeStart = -newRoomScale.X / 2 + 1;
			shiftXRangeEnd = newRoomScale.X / 2 - 1;
		}
		if (newRoomScale.Y > 4) {
			shiftYRangeStart = -newRoomScale.Y / 2 + 1;
			shiftYRangeEnd = newRoomScale.Y / 2 - 1;
		}

		do {
			for (int i = m_nbOfRooms - 1; i >= 0; i--) {
				for (int j = 0; j < 4; j++) { // faces of the previous room that we are checking
					i_face = FMath::RandRange(0, 4);
					newRoomCoordStart = m_rooms[i].coord_start;
					switch (i_face) {
					case 0: newRoomCoordStart.X -= newRoomScale.X; break; // x<0
					case 1: newRoomCoordStart.X = m_rooms[i].coord_end.X; break; // x>0
					case 2: newRoomCoordStart.Y -= newRoomScale.Y; break; // y<0
					case 3: newRoomCoordStart.Y = m_rooms[i].coord_end.Y; break; // y>0
					}
					// We move the new room randomly but it needs to share a wall with m_rooms[i]

					if (i_face < 2) {
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
					i_face++;
					if (i_face >= 4)
						i_face = 0;
				}
				if (!continueSearch)
					break;
			}
			iter_search++;
		} while (continueSearch && iter_search < 10);
		if (iter_search < 10) {
			m_rooms.Add(addRoom(newRoomCoordStart, newRoomCoordEnd, 1));
		}
		iter_search = 0;
		//m_rooms.Add ()... when we will have more types of room

		//We check if all the rooms have been added
		for (int i = 0; i < m_numberOfRoomTypes; i++) {
			if (m_roomTypes[i].current_quantity >= m_roomTypes[i].min_quantity) {
				continueConstruction = false;
			}
		}
		UE_LOG(LogTemp, Log, TEXT("iter = %d | continueSearch= %d | continue construction %d"), iter, continueSearch, continueConstruction);
		iter++;
	} while (iter < 100 && continueConstruction); // iter will be increased when the map gets bigger, it's just a safety for now
}

bool AMapGen::areAlreadyConnected(FIntPoint items, TArray<FIntPoint> connections) {
	for (int i = 0; i < connections.Num(); i++) {
		if ((connections[i][0] == items[0] && connections[i][1] == items[1]) ||
			(connections[i][0] == items[1] && connections[i][1] == items[0]))
		{
			return true;
		}
	}
	return false;
}


void AMapGen::buildDoors() {
	Door newDoor;
	TArray<FIntPoint> connections;
	FIntPoint roomsTested;

	// WE HAVE TO SEARCH IF THERE ARE ALREADY CONNECTIONS
	for (int x = 1; x < m_map.x_width-1; x++) {
		for (int y = 1; y < m_map.y_width-1; y++) {
			if (m_map.grid[x][y] != m_map.grid[x - 1][y]) { // left
				roomsTested.X = m_map.grid[x][y];
				roomsTested.Y = m_map.grid[x - 1][y];
				if (!areAlreadyConnected(roomsTested, connections)) {
					newDoor.face = 0;
					newDoor.coordStart = y;
					newDoor.coordEnd = y + 1;
					m_rooms[m_map.grid[x][y]].doorsArray.Add(newDoor);

					newDoor.face = 1;
					newDoor.coordStart = y + 1;
					newDoor.coordEnd = y;
					m_rooms[m_map.grid[x - 1][y]].doorsArray.Add(newDoor);
					connections.Add(roomsTested);
				}
			}
			if (m_map.grid[x][y] != m_map.grid[x + 1][y]) { // right
				roomsTested.X = m_map.grid[x][y];
				roomsTested.Y = m_map.grid[x + 1][y];
				if (!areAlreadyConnected(roomsTested, connections)) {
					newDoor.face = 1;
					newDoor.coordStart = y + 1;
					newDoor.coordEnd = y;
					m_rooms[m_map.grid[x][y]].doorsArray.Add(newDoor);

					newDoor.face = 0;
					newDoor.coordStart = y;
					newDoor.coordEnd = y + 1;
					m_rooms[m_map.grid[x + 1][y]].doorsArray.Add(newDoor);
					connections.Add(roomsTested);
				}
			}
			if (m_map.grid[x][y] != m_map.grid[x][y - 1]) { // bot
				roomsTested.X = m_map.grid[x][y];
				roomsTested.Y = m_map.grid[x][y - 1];
				if (!areAlreadyConnected(roomsTested, connections)) {
					newDoor.face = 2;
					newDoor.coordStart = x + 1;
					newDoor.coordEnd = x;
					m_rooms[m_map.grid[x][y]].doorsArray.Add(newDoor);

					newDoor.face = 3;
					newDoor.coordStart = x;
					newDoor.coordEnd = x + 1;
					m_rooms[m_map.grid[x][y - 1]].doorsArray.Add(newDoor);
					connections.Add(roomsTested);
				}
			}
			if (m_map.grid[x][y] != m_map.grid[x][y + 1]) { // top
				roomsTested.X = m_map.grid[x][y];
				roomsTested.Y = m_map.grid[x][y + 1];
				if (!areAlreadyConnected(roomsTested, connections)) {
					newDoor.face = 3;
					newDoor.coordStart = x;
					newDoor.coordEnd = x + 1;
					m_rooms[m_map.grid[x][y]].doorsArray.Add(newDoor);

					newDoor.face = 2;
					newDoor.coordStart = x + 1;
					newDoor.coordEnd = x;
					m_rooms[m_map.grid[x][y + 1]].doorsArray.Add(newDoor);
					connections.Add(roomsTested);
				}
			}
		}
	}
}

AMapGen::AMapGen()
{
	m_borders.index = -1;

	m_map.x_width = 50; // in meters
	m_map.y_width = 50;

	m_map.grid = new int* [m_map.x_width];
	for (int i = 0; i < m_map.x_width; i++) {
		m_map.grid[i] = new int[m_map.y_width];
	}

	for (int i = 0; i < m_map.x_width; i++) {
		for (int j = 0; j < m_map.y_width; j++) {
			m_map.grid[i][j] = -1;
		}
	}


	//CORRIDOR
	m_roomTypes[0].min_width = 1; // 1m
	m_roomTypes[0].min_quantity = 5;
	m_roomTypes[0].current_quantity = 0;

	//BASIC ROOM
	m_roomTypes[1].min_width = 2;
	m_roomTypes[1].max_width = 15;
	m_roomTypes[1].min_quantity = 100;
	m_roomTypes[1].current_quantity = 0;
}


// Called when the game starts or when spawned
void AMapGen::BeginPlay()
{
	Super::BeginPlay();	

	m_borders = addRoom({ 0, 0, 0 }, { m_map.x_width, m_map.y_width, 0 }, -1);
	buildRooms();
	//buildCorridors();
	buildDoors();
	generateMesh();	
}

void AMapGen::BeginDestroy() {
	Super::BeginDestroy();
	for (int i = 0; i < m_map.x_width; i++) {
		delete[] m_map.grid[i];
	}
	delete[] m_map.grid;
}
