// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGen.h"
#include "GenericPlatform/GenericPlatformMath.h"



AWall* AMapGen::addWall(FVector start, FVector end, bool isDoor, double height) {
	AWall* newWall;
	newWall = GetWorld()->SpawnActor<AWall>(AWall::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	newWall->m_height = height*100.0;
	newWall->m_start = start;
	newWall->m_end = end;
	if (isDoor) {
		newWall->m_start.Z = 180.0;
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
	UE_LOG(LogTemp, Log, TEXT("m_nbofRooms = %d"), m_nbOfRooms);
	if (newRoom.index != -1) { // if we aren't building the map borders
		for (int x = start.X; x < end.X; x++) {
			for (int y = start.Y; y < end.Y; y++) {
				//UE_LOG(LogTemp, Log, TEXT("add rooms loop : x,y = %d %d   |   max x,y = %d %d"), x, y, m_map.x_width, m_map.y_width);
				m_map.grid[x][y] = newRoom.index + 1; // "+1" to skip the corridors index (0)
			}
		}
	}


	FVector coordStart = FVector(start.X * 100.0, start.Y*100.0, 0.0);
	FVector coordEnd = FVector(end.X * 100.0, end.Y * 100.0, 0.0);
	//UE_LOG(LogTemp, Log, TEXT("add room 1st wall coord : (%lf %lf %lf) -> (%lf %lf %lf)"), coordStart.X, coordStart.Y - m_shift, coordStart.Z, coordEnd.X, coordEnd.Y + m_shift, coordStart.Z);
	newRoom.walls.Add(addWall({ coordStart.X, coordStart.Y - m_shift, coordStart.Z }, { coordStart.X, coordEnd.Y + m_shift, coordStart.Z }));
	newRoom.walls.Add(addWall({ coordStart.X, coordEnd.Y, coordStart.Z }, { coordEnd.X, coordEnd.Y, coordEnd.Z }));
	newRoom.walls.Add(addWall({ coordEnd.X, coordEnd.Y + m_shift, coordEnd.Z }, { coordEnd.X, coordStart.Y - m_shift, coordStart.Z }));
	newRoom.walls.Add(addWall({ coordEnd.X, coordStart.Y, coordStart.Z }, { coordStart.X, coordStart.Y, coordStart.Z }));

	

	UE_LOG(LogTemp, Log, TEXT("ADD THE ROOM N° %d: S(%d %d) | E(%d %d)"), m_nbOfRooms, start.X, start.Y, end.X, end.Y);
	if(type>0)
		m_roomTypes[type].current_quantity++;

	m_nbOfRooms++;
	return newRoom;
}

// the function underneath has to be improved
void AMapGen::generateMesh() {
	for (int j = 0; j < m_borders.walls.Num(); j++) {
		m_borders.walls[j]->createWall();
	}

	for (int i = 0; i < m_rooms.Num(); i++) {
		for (int j = 0; j < m_rooms[i].walls.Num(); j++) {
			m_rooms[i].walls[j]->createWall();
		}
	}

	for (int i = 0; i < m_corridors.Num(); i++) {
		for (int j = 0; j < m_corridors[i].wallElements.Num(); j++) {
			m_corridors[i].wallElements[j]->createWall();
		}
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
		continueConstruction = true;

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
			if (m_roomTypes[i].current_quantity >= m_roomTypes[i].min_quantity) {
				continueConstruction = false;
			}
		}
		UE_LOG(LogTemp, Log, TEXT("iter = %d | continueSearch= %d | continue construction %d"), iter, continueSearch, continueConstruction);
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

AMapGen::AMapGen()
{
	m_borders.index = -1;

	m_map.x_width = 10; // in meters
	m_map.y_width = 10;

	//CORRIDOR
	m_roomTypes[0].min_width = 1; // 1m
	m_roomTypes[0].current_quantity = 0;

	//BASIC ROOM
	m_roomTypes[1].min_width = 4;
	m_roomTypes[1].max_width = 15;
	m_roomTypes[1].min_quantity = 10;
	m_roomTypes[1].current_quantity = 0;

	m_map.grid = new int* [m_map.x_width];
	for (int i = 0; i < m_map.x_width; i++) {
		m_map.grid[i] = new int[m_map.y_width];
	}
}

void AMapGen::insertDoor(FVector newDoorCoord[2], int roomTested) {
	// We are seeking a wall that has the same orientation as the new door (x = const = newDoor.X or y = const = newDoor.Y) and the other coordinate that isn't constant should be the same as the newDoor
	int wallIndex = -1; // index of the wall where the door will be builded
	if (newDoorCoord[0].X == newDoorCoord[1].X) {
		UE_LOG(LogTemp, Log, TEXT("insertDoor() first case"));

		for (int i = 0; i < m_rooms[roomTested].walls.Num(); i++) {
			double y_min = FGenericPlatformMath::Min(m_rooms[roomTested].walls[i]->m_end.Y, m_rooms[roomTested].walls[i]->m_start.Y);
			double y_max = FGenericPlatformMath::Max(m_rooms[roomTested].walls[i]->m_end.Y, m_rooms[roomTested].walls[i]->m_start.Y);
			if (newDoorCoord[0].X == newDoorCoord[1].X &&
				FMath::Abs(m_rooms[roomTested].walls[i]->m_start.X - newDoorCoord[0].X) <= m_shift &&
				FMath::Abs(m_rooms[roomTested].walls[i]->m_end.X - newDoorCoord[1].X) <= m_shift &&
				newDoorCoord[0].Y <= y_max &&
				newDoorCoord[0].Y >= y_min &&
				newDoorCoord[1].Y <= y_max &&
				newDoorCoord[1].Y >= y_min)
			{
				wallIndex = i;

				if (newDoorCoord[0].Y < newDoorCoord[1].Y) { // left
					newDoorCoord[0].Y += m_shift;
					newDoorCoord[1].Y -= m_shift;
				}
				else { // right
					newDoorCoord[0].Y -= m_shift;
					newDoorCoord[1].Y += m_shift;
				}
				break;
			}
		}
	}
	else if (newDoorCoord[0].Y == newDoorCoord[1].Y) {
		UE_LOG(LogTemp, Log, TEXT("insertDoor() second case"));
		for (int i = 0; i < m_rooms[roomTested].walls.Num(); i++) {
			double x_min = FGenericPlatformMath::Min(m_rooms[roomTested].walls[i]->m_end.X, m_rooms[roomTested].walls[i]->m_start.X);
			double x_max = FGenericPlatformMath::Max(m_rooms[roomTested].walls[i]->m_end.X, m_rooms[roomTested].walls[i]->m_start.X);
			UE_LOG(LogTemp, Log, TEXT("door start = (%lf  %lf) | door end = (%lf  %lf) | wall start = (%lf  %lf) | wall end = (%lf  %lf)"), newDoorCoord[0].X, newDoorCoord[0].Y, newDoorCoord[1].X, newDoorCoord[1].Y, m_rooms[roomTested].walls[i]->m_start.X, m_rooms[roomTested].walls[i]->m_start.Y, m_rooms[roomTested].walls[i]->m_end.X, m_rooms[roomTested].walls[i]->m_end.Y);
			if (newDoorCoord[0].Y == newDoorCoord[1].Y && 
				FMath::Abs(m_rooms[roomTested].walls[i]->m_start.Y - newDoorCoord[0].Y) <= m_shift &&
				FMath::Abs(m_rooms[roomTested].walls[i]->m_end.Y - newDoorCoord[1].Y) <= m_shift &&
				newDoorCoord[0].X <= x_max &&
				newDoorCoord[0].X >= x_min &&
				newDoorCoord[1].X <= x_max &&
				newDoorCoord[1].X >= x_min)
			{
				wallIndex = i;

				if (newDoorCoord[0].X < newDoorCoord[1].X) { // top
					newDoorCoord[0].X += m_shift;
					newDoorCoord[1].X -= m_shift;
				}
				else { // bot
					newDoorCoord[0].X -= m_shift;
					newDoorCoord[1].X += m_shift;
				}
				break;
			}
		}
	}
	
	if (wallIndex != -1) {
		if (m_rooms[roomTested].walls[wallIndex]->m_length > 100.0) { // we have enough space to get a door and keep the rest of the wall
			FVector oldWallEnd = m_rooms[roomTested].walls[wallIndex]->m_end;
			m_rooms[roomTested].walls[wallIndex]->m_end = newDoorCoord[0];
			m_rooms[roomTested].walls.Add(addWall(newDoorCoord[0], newDoorCoord[1], true));
			m_rooms[roomTested].walls.Add(addWall(newDoorCoord[1], oldWallEnd));
		}
		else { // we just consider the whole wall as a door
			m_rooms[roomTested].walls[wallIndex]->m_start.Z = 180.0;
			m_rooms[roomTested].walls[wallIndex]->m_height = m_rooms[roomTested].walls[wallIndex]->m_height - m_rooms[roomTested].walls[wallIndex]->m_start.Z;
		}
		UE_LOG(LogTemp, Log, TEXT("The new door has been inserted"));
	}
	else
		UE_LOG(LogTemp, Log, TEXT("The new door has NOT been inserted"));
}

void AMapGen::buildDoors() {
	TArray<FIntPoint> connections;
	FIntPoint roomsTested;
	FVector newDoorCoord[2];
	FVector temp = { 0.0,0.0,0.0 };
	int newIndex = 0;

	for (int x = 0; x < m_map.x_width; x++) {
		for (int y= 0; y < m_map.y_width; y++) {
			roomsTested.X = m_map.grid[x][y];
			if (m_map.grid[x][y] != -1) {
				if (x - 1 >= 0 && m_map.grid[x][y] != m_map.grid[x - 1][y]) { // Left
					roomsTested.Y = m_map.grid[x - 1][y];
					if (!areAlreadyConnected(roomsTested, connections)) {
						newDoorCoord[0] = { x * 100.0, y * 100.0, 0 };
						newDoorCoord[1] = { x * 100.0, (y + 2) * 100.0, 0.0 };
						UE_LOG(LogTemp, Log, TEXT(" %d ; %d = %d has a door to the left"), x, y, m_map.grid[x][y]);
						insertDoor(newDoorCoord, roomsTested.X);
						if (roomsTested.Y != -1) {
							temp = newDoorCoord[0];
							newDoorCoord[0] = newDoorCoord[1];
							newDoorCoord[1] = temp;
							insertDoor(newDoorCoord, roomsTested.Y);
						}
						connections.Add(roomsTested);
					}
				}
				if (x + 1 < m_map.x_width && m_map.grid[x][y] != m_map.grid[x + 1][y]) { // Right
					roomsTested.Y = m_map.grid[x + 1][y];
					if (!areAlreadyConnected(roomsTested, connections)) {
						newDoorCoord[0] = { (x + 1) * 100.0, y * 100.0, 0 };
						newDoorCoord[1] = { (x + 1) * 100.0, (y + 2) * 100.0, 0.0 };
						UE_LOG(LogTemp, Log, TEXT(" %d ; %d = %d has a door to the right"), x, y, m_map.grid[x][y]);
						insertDoor(newDoorCoord, roomsTested.X);
						if (roomsTested.Y != -1) {
							temp = newDoorCoord[0];
							newDoorCoord[0] = newDoorCoord[1];
							newDoorCoord[1] = temp;
							insertDoor(newDoorCoord, roomsTested.Y);
						}
						connections.Add(roomsTested);
					}
				}
				if (y - 1 >= 0 && m_map.grid[x][y] != m_map.grid[x][y - 1]) { // Bot
					roomsTested.Y = m_map.grid[x][y - 1];
					if (!areAlreadyConnected(roomsTested, connections)) {
						newDoorCoord[0] = { x * 100.0, y * 100.0, 0 };
						newDoorCoord[1] = { (x + 2) * 100.0, y * 100.0, 0.0 };
						UE_LOG(LogTemp, Log, TEXT(" %d ; %d = %d has a door to the bot"), x, y, m_map.grid[x][y]);
						insertDoor(newDoorCoord, roomsTested.X);
						if (roomsTested.Y != -1) {
							temp = newDoorCoord[0];
							newDoorCoord[0] = newDoorCoord[1];
							newDoorCoord[1] = temp;
							insertDoor(newDoorCoord, roomsTested.Y);
						}
						connections.Add(roomsTested);
					}
				}
				if (y + 1 < m_map.y_width && m_map.grid[x][y] != m_map.grid[x][y + 1]) { // Top
					roomsTested.Y = m_map.grid[x][y + 1];
					if (!areAlreadyConnected(roomsTested, connections)) {
						newDoorCoord[0] = { x * 100.0, (y + 1) * 100.0, 0 };
						newDoorCoord[1] = { (x + 2) * 100.0, (y + 1) * 100.0, 0.0 };
						UE_LOG(LogTemp, Log, TEXT(" %d ; %d = %d has a door to the top"), x, y, m_map.grid[x][y]);
						insertDoor(newDoorCoord, roomsTested.X);
						if (roomsTested.Y != -1) {
							temp = newDoorCoord[0];
							newDoorCoord[0] = newDoorCoord[1];
							newDoorCoord[1] = temp;
							insertDoor(newDoorCoord, roomsTested.Y);
						}
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
		UE_LOG(LogTemp, Log, TEXT("in getValidNeighbour() : cell and newCell pos : (%d %d) | (%d %d) ||| cell and newCell in map grid %d | %d"), cell.X, cell.Y, newCell->X, newCell->Y, m_map.grid[cell.X][cell.Y], m_map.grid[newCell->X][newCell->Y]);
		if (newCell->X >= 0 && newCell->Y >= 0 && newCell->X < m_map.x_width && newCell->Y < m_map.y_width && m_map.grid[newCell->X][newCell->Y] == -1)
			return direction;

		direction++;
		if (direction > 3)
			direction = 0;
	}
	UE_LOG(LogTemp, Log, TEXT("in getValidNeighbour() : neighbour not found"));
	return -1; // No valid neighbour was found
}

void AMapGen::initializePath(Path *path)
{
	path->pos = FIntPoint::ZeroValue;
	for (int i = 0; i < 4; i++)
		path->walls[i] = true; // there are 4 walls by default
}

void AMapGen::buildCorridors()
{
	Path currentCell;
	initializePath(&currentCell);
	Path newCell;
	int index = 0;
	int direction = 0;
	int oppositeDirection = 0;
	int iter = 0;
	currentCell.pos = getRandomPoint(0, m_map.x_width - 1, 0, m_map.y_width - 1);
	m_corridors.Add(currentCell);
	m_map.grid[currentCell.pos.X][currentCell.pos.Y] = 0;

	do {
		initializePath(&newCell);
		direction = getValidNeighbour(currentCell.pos, &(newCell.pos));

		if (direction == -1) { // No neighbour was found
			UE_LOG(LogTemp, Log, TEXT("neighbour not found, we will search it with a previous cell"));
			index = FMath::RandRange(-1, m_corridors.Num()-2);
			for (int i = 0; i < m_corridors.Num(); i++) {
				index++;
				if (i >= m_corridors.Num())
					index = 0;
				currentCell = m_corridors[index];
				direction = getValidNeighbour(currentCell.pos, &(newCell.pos));

				if (direction != -1) { // A cell with a valid neigbour was found
					break;
				}
			}
		}
		if (direction != -1) {
			UE_LOG(LogTemp, Log, TEXT("neighbour found"));
			oppositeDirection = direction + 2;
			if (oppositeDirection >= 4)
				oppositeDirection -= 4;
			m_corridors[index].walls[direction] = false;
			newCell.walls[oppositeDirection] = false;
			UE_LOG(LogTemp, Log, TEXT("test direction & oppDir in maze gen: %d | %d ||| pos of current & new path: (%d %d) | (%d %d)"), direction, oppositeDirection, currentCell.pos.X, currentCell.pos.Y, newCell.pos.X, newCell.pos.Y);
			m_corridors.Add(newCell);
			m_map.grid[newCell.pos.X][newCell.pos.Y] = 0;
			index = m_corridors.Num() - 1;
			currentCell = m_corridors[index]; // newCell = previous cell
		}
		UE_LOG(LogTemp, Log, TEXT("ITER : %d"), iter);
		iter++;
	} while (iter<10 && m_corridors.Num()< m_map.x_width * m_map.y_width);

	for (int i = 0; i < m_corridors.Num(); i++) {
		UE_LOG(LogTemp, Log, TEXT("corridor %d at pos %d %d: "), i, m_corridors[i].pos.X, m_corridors[i].pos.Y);
		if (m_corridors[i].walls[0]) { // left
			UE_LOG(LogTemp, Log, TEXT("Has a wall on the left side"));
			m_corridors[i].wallElements.Add(addWall({ m_corridors[i].pos.X * 100.0, m_corridors[i].pos.Y * 100.0 - m_shift, 0.0 }, { m_corridors[i].pos.X * 100.0, (m_corridors[i].pos.Y + 1) * 100.0 - m_shift, 0.0 }));
		}
		if (m_corridors[i].walls[1]) { // top
			UE_LOG(LogTemp, Log, TEXT("Has a wall on the top side"));
			m_corridors[i].wallElements.Add(addWall({ m_corridors[i].pos.X * 100.0, (m_corridors[i].pos.Y + 1) * 100.0 - m_shift, 0.0 }, { (m_corridors[i].pos.X + 1) * 100.0, (m_corridors[i].pos.Y + 1) * 100.0 - m_shift, 0.0 }));
		}
		if (m_corridors[i].walls[2]) { // right
			UE_LOG(LogTemp, Log, TEXT("Has a wall on the right side"));
			m_corridors[i].wallElements.Add(addWall({ (m_corridors[i].pos.X + 1) * 100.0, (m_corridors[i].pos.Y + 1) * 100.0 - m_shift, 0.0 }, { (m_corridors[i].pos.X + 1) * 100.0, m_corridors[i].pos.Y * 100.0 - m_shift, 0.0 }));
		}
		if (m_corridors[i].walls[3]) { // bot
			UE_LOG(LogTemp, Log, TEXT("Has a wall on the bot side"));
			m_corridors[i].wallElements.Add(addWall({ (m_corridors[i].pos.X + 1) * 100.0, m_corridors[i].pos.Y * 100.0 - m_shift, 0.0 }, { m_corridors[i].pos.X * 100.0, m_corridors[i].pos.Y * 100.0 - m_shift, 0.0 }));
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

	m_rooms.Empty();
	m_corridors.Empty();

	m_borders.walls.Empty();

	m_borders = addRoom({ 0, 0 }, { m_map.x_width, m_map.y_width }, -1);
	UE_LOG(LogTemp, Log, TEXT("Borders have been added"));
	buildCorridors();
	//buildRooms();
	//UE_LOG(LogTemp, Log, TEXT("m_nbOfRooms %d | m_rooms.Num() %d"), m_nbOfRooms, m_rooms.Num());
	//for (int i = 0; i < m_rooms.Num(); i++) {
		//UE_LOG(LogTemp, Log, TEXT("m_rooms[i].walls.Num() %d"), m_rooms[i].walls.Num());
		//for (int j = 0; j < m_rooms[i].walls.Num(); j++) {
			//UE_LOG(LogTemp, Log, TEXT("TEST"));
			//UE_LOG(LogTemp, Log, TEXT("CREATED WALL coord : (%lf %lf %lf) -> (%lf %lf %lf)"), m_rooms[i].walls[j]->m_start.X, m_rooms[i].walls[j]->m_start.Y, m_rooms[i].walls[j]->m_start.Z, m_rooms[i].walls[j]->m_end.X, m_rooms[i].walls[j]->m_end.Y, m_rooms[i].walls[j]->m_end.Z);
		//}
	//}
	//buildDoors();

	generateMesh();	
}

void AMapGen::BeginDestroy() {
	Super::BeginDestroy();
	for (int i = 0; i < m_map.x_width; i++) {
		delete[] m_map.grid[i];
	}
	delete[] m_map.grid;
}
