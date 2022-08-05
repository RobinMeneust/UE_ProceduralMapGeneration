// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGen.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "HAL/UnrealMemory.h"

AWall* AMapGen::addWall(FVector start, FVector end, bool isDoor, double height) {
	AWall* newWall;
	newWall = GetWorld()->SpawnActor<AWall>(AWall::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	newWall->m_height = height * 100.0;
	newWall->m_start = start;
	newWall->m_end = end;
	if (isDoor) {
		newWall->m_start.Z = 210.0;
		newWall->m_end.Z = newWall->m_start.Z;
		newWall->m_height = newWall->m_height - newWall->m_start.Z;
	}
	newWall->m_length = FVector::Distance(start, end);
	//UE_LOG(LogTemp, Log, TEXT("CREATED WALL coord : (%lf %lf %lf) -> (%lf %lf %lf)"), start.X, start.Y, start.Z, end.X, end.Y, end.Z);
	//UE_LOG(LogTemp, Log, TEXT("CREATED WALL coord : (%lf %lf %lf) -> (%lf %lf %lf)"), newWall->m_start.X, newWall->m_start.Y, newWall->m_start.Z, newWall->m_end.X, newWall->m_end.Y, newWall->m_end.Z);
	return newWall;
}

Room AMapGen::addRoom(FIntPoint start, FIntPoint end, int type) {
	Room newRoom;
	newRoom.index = m_nbOfRooms;
	newRoom.type = type;
	UE_LOG(LogTemp, Log, TEXT("m_nbofRooms = %d"), m_nbOfRooms);
	if (newRoom.index != -1) { // if we aren't building the map borders
		for (int x = start.X; x < end.X; x++) {
			for (int y = start.Y; y < end.Y; y++) {
				//UE_LOG(LogTemp, Log, TEXT("add rooms loop : x,y = %d %d   |   max x,y = %d %d"), x, y, m_map.x_width, m_map.y_width);
				m_map.grid[x][y] = newRoom.index;
			}
		}
	}


	FVector coordStart = FVector(start.X * 100.0, start.Y * 100.0, 0.0);
	FVector coordEnd = FVector(end.X * 100.0, end.Y * 100.0, 0.0);
	//UE_LOG(LogTemp, Log, TEXT("add room 1st wall coord : (%lf %lf %lf) -> (%lf %lf %lf)"), coordStart.X, coordStart.Y - m_shift, coordStart.Z, coordEnd.X, coordEnd.Y + m_shift, coordStart.Z);

	for (int x = start.X; x < end.X; x++) { // Horizontal
		m_map.walls[x][start.Y][0] = 1; // bot
		m_map.walls[x][end.Y][0] = 1; // top
	}
	for (int y = start.Y; y < end.Y; y++) { // Vertical
		m_map.walls[start.X][y][1] = 1; // left
		m_map.walls[end.X][y][1] = 1; // right
	}

	UE_LOG(LogTemp, Log, TEXT("ADD THE ROOM N° %d: S(%d %d) | E(%d %d)"), m_nbOfRooms, start.X, start.Y, end.X, end.Y);
	if (type > 0)
		m_roomTypes[type].current_quantity++;

	m_nbOfRooms++;
	return newRoom;
}

void AMapGen::generateMesh() {
	int start = 0;
	int end = 0;
	FVector vect_start, vect_end;
	bool isDoor = false;
	vect_start.Z = 0.0;
	vect_end.Z = 0.0;

	for (int x = 0; x <= m_map.x_width; x++) { // We create vertical walls
		int y = 0;
		do{
			if (m_map.walls[x][y][1] > 0) {
				start = y;
				if (m_map.walls[x][y][1] == 1) {
					while (y <= m_map.y_width && m_map.walls[x][y][1] == 1) {
						y++;
					}
				}
				else if (m_map.walls[x][y][1] == 2) {
					y++;
					isDoor = true;
				}

				if (y > m_map.y_width)
					y = m_map.y_width;
				end = y;

				// We don't want meshes to overlap
				if (start > 0 && m_map.walls[x][start - 1][1]) { // it's next to another wall with the same direction
					vect_start.Y = start * 100.0;
				}
				else {
					if (!(m_map.walls[x][start][0] && (x > 0 && m_map.walls[x - 1][start][0]))) // Just 1 wall or an intersection between 2 walls
						vect_start.Y = start * 100.0 - m_shift;
					else // Intersection between 3 walls
						vect_start.Y = start * 100.0 + m_shift;
				}

				if (m_map.walls[x][end][1]) { // it's next to another wall with the same direction
					vect_end.Y = end * 100.0;
				}
				else {
					if (!(m_map.walls[x][end][0] && (x > 0 && m_map.walls[x - 1][end][0]))) // Just 1 wall or an intersection between 2 walls
						vect_end.Y = end * 100.0 + m_shift;
					else // Intersection between 3 walls
						vect_end.Y = end * 100.0 - m_shift;
				}
				
				vect_start.X = x * 100.0;
				vect_end.X = x * 100.0;

				if (isDoor) {
					FVector doorStart = vect_start + FVector(0.0, m_shift, 0.0);
					FVector doorEnd = vect_end - FVector(0.0, m_shift, 0.0);
					m_map.wallElements.Add(addWall(vect_start, doorStart));
					m_map.wallElements.Add(addWall(doorStart, doorEnd, isDoor));
					m_map.wallElements.Add(addWall(doorEnd, vect_end));
				}
				else
					m_map.wallElements.Add(addWall(vect_start, vect_end));
				isDoor = false;
			}
			else
				y++;
		}while (y <= m_map.y_width);
	}

	for (int y = 0; y <= m_map.y_width; y++) { // We create horizontal walls
		int x = 0;
		do {
			if (m_map.walls[x][y][0] > 0) {
				start = x;
				if (m_map.walls[x][y][0] == 1) {
					while (x <= m_map.x_width && m_map.walls[x][y][0] == 1) {
						x++;
					}
				}
				else if (m_map.walls[x][y][0] == 2) {
					x++;
					isDoor = true;
				}

				if (x > m_map.x_width)
					x = m_map.x_width;
				end = x;

				// We don't want meshes to overlap
				if (start > 0 && m_map.walls[start - 1][y][0]) { // it's next to another wall with the same direction
					vect_start.X = start * 100.0;
				}
				else {
					if (m_map.walls[start][y][1] || (y > 0 && m_map.walls[start][y - 1][1])) // An intersection between 2 or 3 walls
						vect_start.X = start * 100.0 + m_shift;
					else // Just 1 wall
						vect_start.X = start * 100.0 - m_shift;
				}

				if (m_map.walls[end][y][0]) { // it's next to another wall with the same direction
					vect_end.X = end * 100.0;
				}
				else {
					if (m_map.walls[end][y][1] || (y > 0 && m_map.walls[end][y - 1][1])) // An intersection between 2 or 3 walls
						vect_end.X = end * 100.0 - m_shift;
					else // Just 1 wall
						vect_end.X = end * 100.0 + m_shift;
				}

				vect_start.Y = y * 100.0;
				vect_end.Y = y * 100.0;

				if (isDoor) {
					FVector doorStart = vect_start + FVector(m_shift, 0.0, 0.0);
					FVector doorEnd = vect_end - FVector(m_shift, 0.0, 0.0);
					m_map.wallElements.Add(addWall(vect_start, doorStart));
					m_map.wallElements.Add(addWall(doorStart, doorEnd, isDoor));
					m_map.wallElements.Add(addWall(doorEnd, vect_end));
				}
				else
					m_map.wallElements.Add(addWall(vect_start, vect_end));
				isDoor = false;
			}
			else
				x++;
		} while (x <= m_map.x_width);
	}

	for (int i = 0; i < m_map.wallElements.Num(); i++) {
		m_map.wallElements[i]->createWall();
	}
}

bool AMapGen::isAFreeSpace(FIntPoint coordStart, FIntPoint coordEnd) {
	// IS OUT OF THE MAP
	if (coordStart.X < 0 ||
		coordEnd.X > m_map.x_width ||
		coordStart.Y < 0 ||
		coordEnd.Y > m_map.y_width)
	{
		//UE_LOG(LogTemp, Log, TEXT("is out of the map | S(%d %d %d) | E(%d %d %d)"), coordStart.X, coordStart.Y, coordStart.Z, coordEnd.X, coordEnd.Y, coordEnd.Z);
		return false;
	}

	// IS OVERLAPPING WITH OTHER ROOMS
	for (int x = coordStart.X; x < coordEnd.X; x++) {
		for (int y = coordStart.Y; y < coordEnd.Y; y++) {
			if (m_map.grid[x][y] != -1) {
				//UE_LOG(LogTemp, Log, TEXT("is overlapping | S(%d %d %d) | E(%d %d %d)"), coordStart.X, coordStart.Y, coordStart.Z, coordEnd.X, coordEnd.Y, coordEnd.Z);
				return false;
			}
		}
	}
	//UE_LOG(LogTemp, Log, TEXT("is not out of the map and is not overlapping | S(%d %d %d) | E(%d %d %d)"), coordStart.X, coordStart.Y, coordStart.Z, coordEnd.X, coordEnd.Y, coordEnd.Z);
	return true;
}

FIntPoint AMapGen::getRandomPoint(int x_min, int x_max, int y_min, int y_max)
{
	FIntPoint coord;
	coord.X = FMath::RandRange(x_min, x_max);
	coord.Y = FMath::RandRange(y_min, y_max);

	return coord;
}

void AMapGen::buildRooms()
{
	int iter = 0;
	int iter_search = 0;
	bool continueConstruction = true;
	bool continueSearch = true;
	FIntPoint newRoomCoordStart = { 0, 0 };
	FIntPoint newRoomCoordEnd = { FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width), FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width) };
	FIntPoint newRoomScale;
	TArray<FIntPoint> emptyNeighbours;

	do {
		continueSearch = true;
		continueConstruction = false;

		do {
			newRoomCoordStart = getRandomPoint(0, m_map.x_width - newRoomScale.X, 0, m_map.y_width - newRoomScale.Y);
			newRoomScale = { FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width), FMath::RandRange(m_roomTypes[1].min_width, m_roomTypes[1].max_width) };
			newRoomCoordEnd = newRoomCoordStart + newRoomScale;
			if (isAFreeSpace(newRoomCoordStart, newRoomCoordEnd)) {
				continueSearch = false;
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
			if (m_roomTypes[i].current_quantity < m_roomTypes[i].min_quantity) {
				continueConstruction = true;
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("iter = %d | continueSearch= %d | continue construction %d"), iter, continueSearch, continueConstruction);
		iter++;
	} while (iter < 1000 && continueConstruction); // iter will be increased when the map gets bigger, it's just a safety for now
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
	TArray<FIntPoint> connections;
	FIntPoint roomsTested;
	FVector newDoorCoord[2];
	FVector temp = { 0.0,0.0,0.0 };
	int newIndex = 0;

	for (int x = 0; x < m_map.x_width; x++) {
		for (int y = 0; y < m_map.y_width; y++) {
			roomsTested.X = m_map.grid[x][y];
			if (m_map.grid[x][y] != -1) { // is not empty
				if (x - 1 >= 0 && m_map.grid[x][y] != m_map.grid[x - 1][y]) { // Left
					roomsTested.Y = m_map.grid[x - 1][y];
					if (!areAlreadyConnected(roomsTested, connections)) {
						newDoorCoord[0] = { x * 100.0, y * 100.0, 0 };
						newDoorCoord[1] = { x * 100.0, (y + 1) * 100.0, 0.0 };
						UE_LOG(LogTemp, Log, TEXT(" %d ; %d = %d has a door to the left"), x, y, m_map.grid[x][y]);
						m_map.walls[x][y][1] = 2;
						connections.Add(roomsTested);
					}
				}
				
				if (x + 1 < m_map.x_width && m_map.grid[x][y] != m_map.grid[x + 1][y]) { // Right
					roomsTested.Y = m_map.grid[x + 1][y];
					if (!areAlreadyConnected(roomsTested, connections)) {
						newDoorCoord[0] = { (x + 1) * 100.0, (y + 1) * 100.0, 0 };
						newDoorCoord[1] = { (x + 1) * 100.0, y * 100.0, 0.0 };
						UE_LOG(LogTemp, Log, TEXT(" %d ; %d = %d has a door to the right"), x, y, m_map.grid[x][y]);
						m_map.walls[x + 1][y][1] = 2;
						connections.Add(roomsTested);
					}
				}
				if (y - 1 >= 0 && m_map.grid[x][y] != m_map.grid[x][y - 1]) { // Bot
					roomsTested.Y = m_map.grid[x][y - 1];
					if (!areAlreadyConnected(roomsTested, connections)) {
						newDoorCoord[0] = { (x + 2) * 100.0, y * 100.0, 0 };
						newDoorCoord[1] = { x * 100.0, y * 100.0, 0.0 };
						UE_LOG(LogTemp, Log, TEXT(" %d ; %d = %d has a door to the bot"), x, y, m_map.grid[x][y]);
						m_map.walls[x][y][0] = 2;
						connections.Add(roomsTested);
					}
				}
				if (y + 1 < m_map.y_width && m_map.grid[x][y] != m_map.grid[x][y + 1]) { // Top
					roomsTested.Y = m_map.grid[x][y + 1];
					if (!areAlreadyConnected(roomsTested, connections)) {
						newDoorCoord[0] = { x * 100.0, (y + 1) * 100.0, 0 };
						newDoorCoord[1] = { (x + 2) * 100.0, (y + 1) * 100.0, 0.0 };
						UE_LOG(LogTemp, Log, TEXT(" %d ; %d = %d has a door to the top"), x, y, m_map.grid[x][y]);
						m_map.walls[x][y + 1][0] = 2;
						connections.Add(roomsTested);
					}
				}
			}
		}
	}
}

int AMapGen::getValidNeighbour(FIntPoint cell, FIntPoint* newCell)
{
	int direction = FMath::RandRange(0, 3);

	for (int i = 0; i < 4; i++) {
		*newCell = FIntPoint(cell);
		switch (direction) {
			case 0:	newCell->X--; break; // Left
			case 1:	newCell->Y++; break; // Top
			case 2:	newCell->X++; break; // Right
			case 3:	newCell->Y--; break; // Bot
		}
		//UE_LOG(LogTemp, Log, TEXT("in getValidNeighbour() : cell and newCell pos : (%d %d) | (%d %d)"), cell.X, cell.Y, newCell->X, newCell->Y);
		if (newCell->X >= 0 && newCell->Y >= 0 && newCell->X < m_map.x_width && newCell->Y < m_map.y_width && m_map.grid[newCell->X][newCell->Y] == -1) {
			//UE_LOG(LogTemp, Log, TEXT("in getValidNeighbour() : cell and newCell in map grid %d | %d"), newCell->Y, m_map.grid[cell.X][cell.Y], m_map.grid[newCell->X][newCell->Y]);
			return direction;
		}

		direction++;
		if (direction > 3)
			direction = 0;
	}
	//UE_LOG(LogTemp, Log, TEXT("in getValidNeighbour() : neighbour not found"));
	return -1; // No valid neighbour was found
}

void AMapGen::initializeCellWalls(FIntPoint cell, char type)
{
	m_map.walls[cell.X][cell.Y][0] = type;
	m_map.walls[cell.X][cell.Y][1] = type;
	m_map.walls[cell.X][cell.Y  + 1][0] = type;
	m_map.walls[cell.X + 1][cell.Y][1] = type;
}

void AMapGen::removeWall(FIntPoint cell, int direction)
{
	//UE_LOG(LogTemp, Log, TEXT("removeWall() X | Y = %d | %d ||| direction = %d"), cell.X, cell.Y, direction);
	switch (direction) {
	case 0:	// Left
		//UE_LOG(LogTemp, Log, TEXT("LEFT -> index of m_rooms : %d / %d ||| TYPE = %d"), m_map.grid[cell.X - 1][cell.Y], m_rooms.Num(), m_rooms[m_map.grid[cell.X - 1][cell.Y]].type);
		if (cell.X > 0 && (m_map.grid[cell.X - 1][cell.Y] == -1 || m_rooms[m_map.grid[cell.X - 1][cell.Y]].type == 0)) { // We don't remove map borders and room walls
			m_map.walls[cell.X][cell.Y][1] = 0;
		}
		break;
	case 1: // Top
		//UE_LOG(LogTemp, Log, TEXT("TOP -> index of m_rooms : %d / %d ||| TYPE = %d"), m_map.grid[cell.X][cell.Y + 1], m_rooms.Num(), m_rooms[m_map.grid[cell.X][cell.Y + 1]].type);
		if (cell.Y + 1 < m_map.y_width && (m_map.grid[cell.X][cell.Y + 1] == -1 || m_rooms[m_map.grid[cell.X][cell.Y + 1]].type == 0)) {
			m_map.walls[cell.X][cell.Y + 1][0] = 0;
		}
		break;
	case 2: // Right
		//UE_LOG(LogTemp, Log, TEXT("RIGHT -> index of m_rooms : %d / %d ||| TYPE = %d"), m_map.grid[cell.X + 1][cell.Y], m_rooms.Num(), m_rooms[m_map.grid[cell.X + 1][cell.Y]].type);
		if (cell.X + 1 < m_map.x_width && (m_map.grid[cell.X + 1][cell.Y] == -1 || m_rooms[m_map.grid[cell.X + 1][cell.Y]].type == 0)) {
			m_map.walls[cell.X + 1][cell.Y][1] = 0;
		}
		break;
	case 3:	// Bot
		//UE_LOG(LogTemp, Log, TEXT("BOT -> index of m_rooms : %d / %d ||| TYPE = %d"), m_map.grid[cell.X][cell.Y - 1], m_rooms.Num(), m_rooms[m_map.grid[cell.X][cell.Y - 1]].type);
		if (cell.Y > 0 && (m_map.grid[cell.X][cell.Y - 1] == -1 || m_rooms[m_map.grid[cell.X][cell.Y - 1]].type == 0)) {
			m_map.walls[cell.X][cell.Y][0] = 0;
		}
		break; 
	}
}

void AMapGen::buildCorridors()
{
	FIntPoint currentCell;
	FIntPoint newCell;
	int index = 0;
	int direction = 0;
	int oppositeDirection = 0;
	bool firstCell = true;
	int iter = 0;
	int prev = -1;
	int max_iter = m_map.x_width * m_map.y_width;
	Room corridor;
	corridor.index = m_rooms.Num() - 1;
	corridor.type = 0;

	for (int x = 0; x < m_map.x_width; x++) {
		for (int y = 0; y < m_map.y_width; y++) {
			//UE_LOG(LogTemp, Log, TEXT("x | y = %d | %d"), x, y);
			if (m_map.grid[x][y] == -1) {
				if (prev != iter) { // We add a corridor (it will be used to link unconnected) paths
					m_rooms.Add(corridor);
					corridor.index++;
					m_nbOfRooms++;
					prev = iter;
				}
				currentCell = FIntPoint(x, y);
				initializeCellWalls(currentCell, 1);
				do {
					newCell = FIntPoint::ZeroValue;
					direction = getValidNeighbour(currentCell, &newCell);
					if (firstCell) {
						m_corridors.Add(currentCell);
						m_map.grid[currentCell.X][currentCell.Y] = corridor.index;
						firstCell = false;
					}

					if (direction == -1) { // No neighbour was found
						//UE_LOG(LogTemp, Log, TEXT("neighbour not found, we will search it with a previous cell"));
						index = FMath::RandRange(-1, m_corridors.Num() - 2);
						for (int i = 0; i < m_corridors.Num(); i++) {
							index++;
							if (index >= m_corridors.Num())
								index = 0;
							currentCell = m_corridors[index];
							direction = getValidNeighbour(currentCell, &newCell);

							if (direction != -1) { // A cell with a valid neigbour was found
								break;
							}
						}
					}

					if (direction != -1) {
						//UE_LOG(LogTemp, Log, TEXT("neighbour found"));
						initializeCellWalls(newCell, 1);
						m_map.grid[newCell.X][newCell.Y] = corridor.index;
						m_corridors.Add(newCell);
						oppositeDirection = direction + 2;
						if (oppositeDirection >= 4)
							oppositeDirection -= 4;
						removeWall(m_corridors[index], direction);
						removeWall(newCell, oppositeDirection);
						//UE_LOG(LogTemp, Log, TEXT("test direction & oppDir in maze gen: %d | %d ||| pos of current & new path: (%d %d) | (%d %d) ||| m_corridors[index] = (%d %d)"), direction, oppositeDirection, currentCell.X, currentCell.Y, newCell.X, newCell.Y, m_corridors[index].X, m_corridors[index].Y);
						//UE_LOG(LogTemp, Log, TEXT("_______________________________________"));
						index = m_corridors.Num() - 1;
						currentCell = m_corridors[index]; // newCell = previous cell
					}
					else
						break;
					iter++;
				} while (iter < max_iter);
			}
		}
	}
}

void AMapGen::displayMapInConsole() {
	FString txt;
	txt = "";
	for (int y = m_map.y_width; y >= 0; y--) {
		for (int x = 0; x <= m_map.x_width; x++) {
			if (m_map.walls[x][y][1] == 1)
				txt += "|";
			else if(m_map.walls[x][y][1] == 2)
				txt += ".";
			else
				txt += " ";
			if (m_map.walls[x][y][0] == 1)
				txt += "_";
			else if (m_map.walls[x][y][0] == 2)
				txt += ".";
			else
				txt += " ";
		}
		UE_LOG(LogTemp, Log, TEXT("%s (y%d)"), *txt, y);
		txt = "";
	}
}

AMapGen::AMapGen()
{
	m_borders.index = -1;

	m_map.x_width = 50; // in meters
	m_map.y_width = 50;

	//CORRIDOR
	m_roomTypes[0].min_width = 1; // 1m
	m_roomTypes[0].current_quantity = 0;

	//BASIC ROOM
	m_roomTypes[1].min_width = 4;
	m_roomTypes[1].max_width = 10;
	m_roomTypes[1].min_quantity = 40;
	m_roomTypes[1].current_quantity = 0;

	m_map.grid = (int**) FMemory::Malloc(sizeof(int*) * m_map.x_width);
	for (int i = 0; i < m_map.x_width; i++) {
		m_map.grid[i] = (int*)FMemory::Malloc(sizeof(int) * m_map.y_width);
	}

	m_map.walls = (char***)FMemory::Malloc(sizeof(char**) * (m_map.x_width+1));
	for (int i = 0; i <= m_map.x_width; i++) {
		m_map.walls[i] = (char**)FMemory::Malloc(sizeof(char*) * (m_map.y_width + 1));
		for (int j = 0; j <= m_map.y_width; j++) {
			m_map.walls[i][j] = (char*)FMemory::Malloc(sizeof(char) * 2);
		}
	}

}

// Called when the game starts or when spawned
void AMapGen::BeginPlay()
{
	Super::BeginPlay();
	m_nbOfRooms = -1;
	for (int i = 0; i < m_map.x_width; i++) {
		for (int j = 0; j < m_map.y_width; j++) {
			m_map.grid[i][j] = -1;
		}
	}

	for (int i = 0; i <= m_map.x_width; i++) {
		for (int j = 0; j <= m_map.y_width; j++) {
			// by default there are no walls on the map
			m_map.walls[i][j][0] = 0; // x>0 vector
			m_map.walls[i][j][1] = 0; // y>0 vector
		}
	}

	m_rooms.Empty();
	m_corridors.Empty();

	m_borders = addRoom({0, 0}, {m_map.x_width, m_map.y_width}, -1);
	UE_LOG(LogTemp, Log, TEXT("Borders have been added"));

	buildRooms();
	UE_LOG(LogTemp, Log, TEXT("Rooms have been added"));

	buildCorridors();
	UE_LOG(LogTemp, Log, TEXT("Corridors have been added"));

	buildDoors();
	UE_LOG(LogTemp, Log, TEXT("Doors have been added"));

	UE_LOG(LogTemp, Log, TEXT("m_nbOfRooms %d | m_rooms.Num() %d"), m_nbOfRooms, m_rooms.Num());
	//for (int i = 0; i < m_rooms.Num(); i++) {
		//UE_LOG(LogTemp, Log, TEXT("m_rooms[%d].type = %d"), i, m_rooms[i].type);
	//}
	
	displayMapInConsole();
	
	generateMesh();

	player = GetWorld()->SpawnActor<AFPSCharacter>(AFPSCharacter::StaticClass(), FVector(-100,-100,200), FRotator::ZeroRotator);
}

void AMapGen::BeginDestroy() {
	Super::BeginDestroy();
	if (!m_map.grid) {
		for (int i = 0; i < m_map.x_width; i++) {
			if (!m_map.grid[i]) {
				FMemory::Free(m_map.grid[i]);
			}
		}
		FMemory::Free(m_map.grid);
	}
	m_map.grid = NULL;

	if (m_map.walls != NULL) {
		for (int i = 0; i <= m_map.x_width; i++) {
			if (!m_map.walls[i]) {
				for (int j = 0; j <= m_map.y_width; j++) {
					if (!m_map.walls[i][j])
						FMemory::Free(m_map.walls[i][j]);
				}
				FMemory::Free(m_map.walls[i]);
			}
		}
		FMemory::Free(m_map.walls);
		m_map.walls=NULL;
	}
}