// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMap.h"
#include <cstdio>
#include <Game/bitmap_image.hpp>
#include <vector>
#include <iostream>
#include "AITestsCommon.h"
#include "NodePoint.h"
#include "Waypoint.h"
#include "NavigationSystem.h"
#include "Engine/Engine.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "Engine/World.h"


#define TURN_LU "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/Plane_012_Plane_2350.Plane_012_Plane_2350'";
#define TURN_UR "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/Plane_017_Plane_2355.Plane_017_Plane_2355'";
#define TURN_RD "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/ANGLE__LEFT_DOWN_Plane_2353.ANGLE__LEFT_DOWN_Plane_2353'";
#define TURN_DL "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/ANGLE_LEFT_UP_Plane_2356.ANGLE_LEFT_UP_Plane_2356'";
#define STRAIGHT_VERT "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/VERTICAL_2_Plane_2357.VERTICAL_2_Plane_2357'";
#define STRAIGHT_HORIZ "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/HORIZONTAL_1_Plane_2354.HORIZONTAL_1_Plane_2354'";
#define GRASS_T1 "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/GRASS_0_Plane_2352.GRASS_0_Plane_2352'";
#define PLACED "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/GRASS_0_Plane_2352.GRASS_0_Plane_2352'";
#define SIZER FVector(3.f,3.f,3.f)

AMyMap::AMyMap()
{
	PrimaryActorTick.bCanEverTick = false;
	ConvertMapFromImage();
}

void AMyMap::SetTileSize(int T_Size)
{
	TILE_SIDE_LEN = T_Size;
}

void AMyMap::SpawnMap()
{
	for (int i = 0; i < this->mapHeight; i++)
	{
		for (int j = 0; j < this->mapWidth; j++)
		{
			if (Map_Cells[i][j] == 0)
			{
				FString AssetPathName;
				// TODO: random seed
				float rs = 0.1;
				if (rs < 0.2f)
				{
					AssetPathName = TEXT(GRASS_T1);
				}
				else
				{
					// TODO: other references to tiles
				}

				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				AMyTile* Tile = GetWorld()->SpawnActor<AMyTile>(AMyTile::StaticClass(), SpawnParameters);
				UStaticMesh* Asset = Cast<UStaticMesh>(
					StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));
				Tiles_Cells[i].Add(Tile);
				Tiles_Cells[i][j]->Mesh->SetStaticMesh(Asset);
				Tiles_Cells[i][j]->Mesh->SetWorldScale3D(SIZER);
				Tiles_Cells[i][j]->SceneComponent->SetWorldLocation(FVector(
					i * TILE_SIDE_LEN, j * TILE_SIDE_LEN, 0 * TILE_SIDE_LEN / 2));
			}
			if (Map_Cells[i][j] == 1)
			{
				FString AssetPathName;
				if (j > 0 && Map_Cells[i][j - 1] != 1 && i > 0 && Map_Cells[i - 1][j] != 1)
				{
					// turn ╚    ///  ╗╝╚╔
					AssetPathName = TEXT(TURN_LU);
				}
				else if (j > 0 && Map_Cells[i][j - 1] != 1 && i < mapHeight && Map_Cells[i + 1][j] != 1)
				{
					// turn ╔    ///  ╗╝╚╔
					AssetPathName = TEXT(TURN_UR);
				}
				else if (j < mapWidth && Map_Cells[i][j + 1] != 1 && i < mapHeight && Map_Cells[i + 1][j] != 1)
				{
					// turn ╗    ///  ╗╝╚╔
					AssetPathName = TEXT(TURN_RD);
				}
				else if (j < mapWidth && Map_Cells[i][j + 1] != 1 && i > 0 && Map_Cells[i - 1][j] != 1)
				{
					//  turn ╝    ///  ╗╝╚╔
					AssetPathName = TEXT(TURN_DL);
				}
				else if ((j < mapWidth && Map_Cells[i][j + 1] != 1 && j > 0 && Map_Cells[i][j - 1] != 1) && (i <
					mapHeight && Map_Cells[i + 1][j] == 1 || i == mapHeight) && (i > 0 && Map_Cells[i - 1][j] == 1 || i
					== 0))
				{
					// straight ║   ///  ║═
					AssetPathName = TEXT(STRAIGHT_VERT);
				}
				else if ((j < mapWidth && Map_Cells[i][j + 1] == 1 || j == mapWidth) && (j > 0 && Map_Cells[i][j - 1] ==
					1 || j == 0) && (i < mapHeight && Map_Cells[i + 1][j] != 1 && i > 0 && Map_Cells[i - 1][j] != 1))
				{
					// straight ═   ///  ║═
					AssetPathName = TEXT(STRAIGHT_HORIZ);
				}
				//AssetPathName = TEXT("/Script/Engine.StaticMesh'/Game/_Main/Voxels/path/path.path'");
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				AMyTile* Tile = GetWorld()->SpawnActor<AMyTile>(AMyTile::StaticClass(), SpawnParameters);
				UStaticMesh* Asset = Cast<UStaticMesh>(
					StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));
				Tiles_Cells[i].Add(Tile);
				Tiles_Cells[i][j]->Mesh->SetStaticMesh(Asset);
				Tiles_Cells[i][j]->Mesh->SetWorldScale3D(SIZER);
				Tiles_Cells[i][j]->SceneComponent->SetWorldLocation(FVector(
					i * TILE_SIDE_LEN, j * TILE_SIDE_LEN, 0 * TILE_SIDE_LEN / 2));
			}
			if (Map_Cells[i][j] == -1)
			{
				FString AssetPathName =
					TEXT("/Script/Engine.StaticMesh'/Game/_Main/Voxels/Obstacle/obstacle.obstacle'");
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				AMyTile* Tile = GetWorld()->SpawnActor<AMyTile>(AMyTile::StaticClass(), SpawnParameters);
				UStaticMesh* Asset = Cast<UStaticMesh>(
					StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));

				Tiles_Cells[i].Add(Tile);
				Tiles_Cells[i][j]->Mesh->SetStaticMesh(Asset);
				Tiles_Cells[i][j]->Mesh->SetWorldScale3D(FVector(10.f / 32.f, 10.f / 32.f, 10.f / 32.f));
				Tiles_Cells[i][j]->SceneComponent->SetWorldLocation(FVector(
					i * TILE_SIDE_LEN - TILE_SIDE_LEN / 2, j * TILE_SIDE_LEN + TILE_SIDE_LEN / 2, -TILE_SIDE_LEN));
			}
		}
	}
	/*bool a = GetWorld()->IsNavigationRebuilt();
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT("%i"), a));*/
	UpdateNavMeshik();
}

void AMyMap::UpdateNavMeshik()
{
	// Получаем ссылку на навигационную систему
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(FAITestHelpers::GetWorld());
	if (NavSystem)
	{
		NavSystem->MainNavData->RebuildAll();
	}
}

void AMyMap::ConvertMapFromImage()
{
	///
	/// R -> path for enemies					 {1}value
	///	G -> cells for towers					 {0}value
	/// B -> some kind of bushes for obstacle	{-1}value
	///


	MAP_FILE_NAME = "X:/EpicGames_Unreal/pROJECTs/Game/Source/Game/Beta_map.bmp";
	bitmap_image Map(MAP_FILE_NAME);
	if (!Map)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("FILE DID NOT LOAD((")));

		return;
	}
	this->mapHeight = Map.height();
	this->mapWidth = Map.width();

	Map_Cells = new int*[this->mapHeight];
	for (int i = 0; i < this->mapHeight; i++)
	{
		Tiles_Cells.Add(FTiles2DArray());
		Map_Cells[i] = new int[this->mapWidth];

		for (int j = 0; j < this->mapWidth; j++)
		{
			rgb_t Color;
			Map.get_pixel(j, i, Color);
			int R = Color.red;
			int B = Color.blue;
			int G = Color.green;

			if (R > 230 && G > 230)
			{
				StartCell.X_coord = j;
				StartCell.Y_coord = i;
			}
			if (R > 230)
			{
				Map_Cells[i][j] = 1; /// RED		
			}
			else
			{
				if (G > 230)
				{
					Map_Cells[i][j] = 0; /// GREEN
				}
				else
				{
					if (B > 230)
					{
						Map_Cells[i][j] = -1; /// BLUE
					}
					else
					{
						Map_Cells[i][j] = -2; /// error pixel
					}
				}
			}
		}
	}
}

void AMyMap::ProcessWaypoints()
{
	//Nodes_Pre = new ANodePoint[mapHeight*mapWidth];
	int k = 0;
	int min_x_coord = 1000;
	FVector Direction = FVector::ZeroVector;
	/// find first coord
	int x = StartCell.X_coord;
	int y = StartCell.Y_coord;
	if (x != mapWidth && Map_Cells[y][x + 1] == 1)
	{
		Direction = FVector::RightVector;
		x++;
	}
	else if (x != 0 && Map_Cells[y][x - 1] == 1)
	{
		Direction = FVector::LeftVector;
		x--;
	}
	else if (y != 0 && Map_Cells[y - 1][x] == 1)
	{
		Direction = FVector::UpVector;
		y--;
	}
	else if (y != mapHeight && Map_Cells[y + 1][x] == 1)
	{
		Direction = FVector::DownVector;
		y++;
	}
	int Node_Index = 0;
	while (true)
	{
		if (Direction == FVector::RightVector)
		{
			if (x != mapWidth && Map_Cells[y][x + 1] == 1)
			{
				x++;
				continue;
			}

			if (y != 0 && Map_Cells[y - 1][x] == 1)
			{
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				//ANodePoint* Point = GetWorld()->SpawnActor<ANodePoint>(ANodePoint::StaticClass(), SpawnParameters);

				FCoords Point;
				Nodes.Add(Point);
				Nodes[Node_Index].X_coord = x;
				Nodes[Node_Index].Y_coord = y;
				Nodes[Node_Index].Index = Node_Index;

				Node_Index++;

				Direction = FVector::UpVector;
				y--;


				continue;
			}

			if (y != mapHeight && Map_Cells[y + 1][x] == 1)
			{
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;


				FCoords Point;
				Nodes.Add(Point);
				Nodes[Node_Index].X_coord = x;
				Nodes[Node_Index].Y_coord = y;
				Nodes[Node_Index].Index = Node_Index;
				Node_Index++;
				Direction = FVector::DownVector;
				y++;

				continue;
			}

			EndCell.X_coord = x;
			EndCell.Y_coord = y;

			FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
			SpawnParameters.Owner = this;


			FCoords Point;
			Nodes.Add(Point);
			Nodes[Node_Index].X_coord = x;
			Nodes[Node_Index].Y_coord = y;
			Nodes[Node_Index].Index = Node_Index;
			Node_Index++;
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow,
			                                 FString::Printf(
				                                 TEXT("x: %i, y%i"), EndCell.X_coord, EndCell.Y_coord));
			break;
		}

		if (Direction == FVector::LeftVector)
		{
			if (x != 0 && Map_Cells[y][x - 1] == 1)
			{
				x--;
				continue;
			}

			if (y != 0 && Map_Cells[y - 1][x] == 1)
			{
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				FCoords Point;
				Nodes.Add(Point);
				Nodes[Node_Index].X_coord = x;
				Nodes[Node_Index].Y_coord = y;
				Nodes[Node_Index].Index = Node_Index;
				Node_Index++;
				Direction = FVector::UpVector;
				y--;


				continue;
			}

			if (y != mapHeight && Map_Cells[y + 1][x] == 1)
			{
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				FCoords Point;
				Nodes.Add(Point);
				Nodes[Node_Index].X_coord = x;
				Nodes[Node_Index].Y_coord = y;
				Nodes[Node_Index].Index = Node_Index;
				Node_Index++;

				Direction = FVector::DownVector;
				y++;
				continue;
			}

			EndCell.X_coord = x;
			EndCell.Y_coord = y;

			FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
			SpawnParameters.Owner = this;

			FCoords Point;
			Nodes.Add(Point);
			Nodes[Node_Index].X_coord = x;
			Nodes[Node_Index].Y_coord = y;
			Nodes[Node_Index].Index = Node_Index;
			Node_Index++;

			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue,
			                                 FString::Printf(
				                                 TEXT("x: %i, y%i"), EndCell.X_coord, EndCell.Y_coord));
			break;
		}

		if (Direction == FVector::UpVector)
		{
			if (y != 0 && Map_Cells[y - 1][x] == 1)
			{
				y--;
				continue;
			}
			if (x != 0 && Map_Cells[y][x - 1] == 1)
			{
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				FCoords Point;
				Nodes.Add(Point);
				Nodes[Node_Index].X_coord = x;
				Nodes[Node_Index].Y_coord = y;
				Nodes[Node_Index].Index = Node_Index;
				Node_Index++;

				Direction = FVector::LeftVector;
				x--;

				continue;
			}
			if (x != mapWidth && Map_Cells[y][x + 1] == 1)
			{
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				FCoords Point;
				Nodes.Add(Point);
				Nodes[Node_Index].X_coord = x;
				Nodes[Node_Index].Y_coord = y;
				Nodes[Node_Index].Index = Node_Index;
				Node_Index++;

				Direction = FVector::RightVector;
				x++;
				continue;
			}

			EndCell.X_coord = x;
			EndCell.Y_coord = y;
			FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
			SpawnParameters.Owner = this;

			FCoords Point;
			Nodes.Add(Point);
			Nodes[Node_Index].X_coord = x;
			Nodes[Node_Index].Y_coord = y;
			Nodes[Node_Index].Index = Node_Index;
			Node_Index++;
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red,
			                                 FString::Printf(
				                                 TEXT("x: %i, y%i"), EndCell.X_coord, EndCell.Y_coord));
			break;
		}
		if (Direction == FVector::DownVector)
		{
			if (y != mapHeight && Map_Cells[y + 1][x] == 1)
			{
				y++;
				continue;
			}
			if (x != 0 && Map_Cells[y][x - 1] == 1)
			{
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				FCoords Point;
				Nodes.Add(Point);
				Nodes[Node_Index].X_coord = x;
				Nodes[Node_Index].Y_coord = y;
				Nodes[Node_Index].Index = Node_Index;
				Node_Index++;

				Direction = FVector::LeftVector;
				x--;
				continue;
			}
			if (x != mapWidth && Map_Cells[y][x + 1] == 1)
			{
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				FCoords Point;
				Nodes.Add(Point);
				Nodes[Node_Index].X_coord = x;
				Nodes[Node_Index].Y_coord = y;
				Nodes[Node_Index].Index = Node_Index;
				Node_Index++;

				Direction = FVector::RightVector;
				x++;
				continue;
			}

			EndCell.X_coord = x;
			EndCell.Y_coord = y;

			FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
			SpawnParameters.Owner = this;

			FCoords Point;
			Nodes.Add(Point);
			Nodes[Node_Index].X_coord = x;
			Nodes[Node_Index].Y_coord = y;
			Nodes[Node_Index].Index = Node_Index;
			Node_Index++;

			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White,
			                                 FString::Printf(
				                                 TEXT("x: %i, y%i"), EndCell.X_coord, EndCell.Y_coord));
			break;
		}
		break;
	}
	Nodes_Num = Node_Index + 1;
}


void AMyMap::SpawnWaypoints()
{
	for (auto Node : Nodes)
	{
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.Owner = this;

		AWaypoint* Waypoint = GetWorld()->SpawnActor<AWaypoint>(
			FVector(), FRotator(), SpawnParameters);
		Waypoint->SetActorLocation(FVector(Node.X_coord * TILE_SIDE_LEN, Node.Y_coord * TILE_SIDE_LEN, 250));
		Waypoint->SetWaypoinOrder(Node.Index);
		Waypoint->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	}
}


void AMyMap::MapChange(const int i, const int j, std::string action)
{
	if (action == "PLACED")
	{
		this->Map_Cells[i][j] = 2;

		FString AssetPathName = TEXT(PLACED);
		UStaticMesh* Asset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));

		//Tiles_Cells[i].Add(Tile);

		Tiles_Cells[i][j]->Mesh->SetStaticMesh(Asset);
		Tiles_Cells[i][j]->Mesh->SetWorldScale3D(SIZER);
		//Tiles_Cells[i][j]->SceneComponent->SetWorldLocation(FVector( i * TILE_SIDE_LEN, j * TILE_SIDE_LEN, -200));
	}
	if (action == "DESTROYED")
	{
		this->Map_Cells[i][j] = 0;

		FString AssetPathName = TEXT(GRASS_T1);
		UStaticMesh* Asset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));

		//Tiles_Cells[i].Add(Tile);

		Tiles_Cells[i][j]->Mesh->SetStaticMesh(Asset);
		Tiles_Cells[i][j]->Mesh->SetWorldScale3D(SIZER);
	}
}

// Called when the game starts or when spawned
void AMyMap::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
