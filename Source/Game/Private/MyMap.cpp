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
#include "Tower.h"
#include "Engine/World.h"

#define TURN_LU "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/Plane_012_Plane_2350.Plane_012_Plane_2350'";
#define TURN_UR "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/Plane_017_Plane_2355.Plane_017_Plane_2355'";
#define TURN_RD "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/ANGLE__LEFT_DOWN_Plane_2353.ANGLE__LEFT_DOWN_Plane_2353'";
#define TURN_DL "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/ANGLE_LEFT_UP_Plane_2356.ANGLE_LEFT_UP_Plane_2356'";
#define STRAIGHT_VERT "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/VERTICAL_2_Plane_2357.VERTICAL_2_Plane_2357'";
#define STRAIGHT_HORIZ "/Script/Engine.StaticMesh'/Game/_Main/_ROAD_TILES/fixed/HORIZONTAL_1_Plane_2354.HORIZONTAL_1_Plane_2354'";

#define GRASS_T1 "/Script/Engine.StaticMesh'/Game/_Main/_GRASS_TILES/4_T_Plane_026.4_T_Plane_026'";
#define GRASS_T2 "/Script/Engine.StaticMesh'/Game/_Main/_GRASS_TILES/3_T_Plane_023.3_T_Plane_023'";
#define GRASS_T3 "/Script/Engine.StaticMesh'/Game/_Main/_GRASS_TILES/7_T_Plane_028.7_T_Plane_028'";
#define GRASS_T4 "/Script/Engine.StaticMesh'/Game/_Main/_GRASS_TILES/9_T_Plane_030.9_T_Plane_030'";

#define PLACED "/Script/Engine.StaticMesh'/Game/_Main/_GRASS_TILES/4_T_Plane_026.4_T_Plane_026'";

#define FENCE_OPP "/Script/Engine.StaticMesh'/Game/_Main/_FENCE_TILES/3_Plane_005.3_Plane_005'"
#define FENCE_ANGULAR "/Script/Engine.StaticMesh'/Game/_Main/_FENCE_TILES/4_Plane_010.4_Plane_010'"
#define FENCE_SNGL "/Script/Engine.StaticMesh'/Game/_Main/_FENCE_TILES/2_Plane_011.2_Plane_011'"
#define FENCE_ALLSIDE "/Script/Engine.StaticMesh'/Game/_Main/_FENCE_TILES/5_Plane_007.5_Plane_007'"
#define FENCE_ZERO "/Script/Engine.StaticMesh'/Game/_Main/_FENCE_TILES/1_Plane_012.1_Plane_012'"

#define SIZER FVector(3.f, 3.f, 3.f)

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
			if (Map_Cells[i][j] == static_cast<int>(EMapEnum::Field) || Map_Cells[i][j] == static_cast<int>(EMapEnum::Nest) || Map_Cells[i][j] == static_cast<int>(EMapEnum::Spawner_one) || Map_Cells[i][j] == static_cast<int>(EMapEnum::Spawner_two))
			{
				FString AssetPathName;
				// TODO: random seed
				FRotator Rotator = FRotator(0, 0, 0);
				int32 RandomNumber = FMath::RandRange(0, 3);

				switch (RandomNumber)
				{
				case 0:
					AssetPathName = TEXT(GRASS_T1);
					break;
				case 1:
					AssetPathName = TEXT(GRASS_T2);
					break;
				case 2:
					AssetPathName = TEXT(GRASS_T3);
					break;
				case 3:
					AssetPathName = TEXT(GRASS_T4);
					break;
				default:
					AssetPathName = TEXT(GRASS_T1);
					break;
				}
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				AMyTile *Tile = GetWorld()->SpawnActor<AMyTile>(AMyTile::StaticClass(), SpawnParameters);
				UStaticMesh *Asset = Cast<UStaticMesh>(
					StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));
				Tiles_Cells[i].Add(Tile);
				Tiles_Cells[i][j]->Mesh_L1->SetStaticMesh(Asset);
				Tiles_Cells[i][j]->Mesh_L1->SetWorldScale3D(SIZER);
				Tiles_Cells[i][j]->Mesh_L1->SetWorldRotation(Rotator);
				Tiles_Cells[i][j]->SceneComponent->SetWorldLocation(FVector(
					i * TILE_SIDE_LEN, j * TILE_SIDE_LEN, 0 * TILE_SIDE_LEN / 2));
			}
			if (Map_Cells[i][j] == static_cast<int>(EMapEnum::Path))
			{
				FString AssetPathName;
				if (j > 0 && Map_Cells[i][j - 1] == 1 && i > 0 && Map_Cells[i - 1][j] == 1)
				{
					// turn ╗    ///  ╔╗╚╝
					AssetPathName = TEXT(TURN_RD);
				}
				else if (j > 0 && Map_Cells[i][j - 1] == 1 && i < mapHeight && Map_Cells[i + 1][j] == 1)
				{
					// turn ╝    ///  ╔╗╚╝
					AssetPathName = TEXT(TURN_DL);
				}
				else if (j < mapWidth && Map_Cells[i][j + 1] == 1 && i < mapHeight && Map_Cells[i + 1][j] == 1)
				{
					// turn ╚    ///  ╔╗╚╝
					AssetPathName = TEXT(TURN_LU);
				}
				else if (j < mapWidth && Map_Cells[i][j + 1] == 1 && i > 0 && Map_Cells[i - 1][j] == 1)
				{
					//  turn ╔    ///  ╔╗╚╝
					AssetPathName = TEXT(TURN_UR);
				}
				else if ((j < mapWidth && Map_Cells[i][j + 1] != 1 && j > 0 && Map_Cells[i][j - 1] != 1) && (i < mapHeight && Map_Cells[i + 1][j] == 1 || i == mapHeight) && (i > 0 && Map_Cells[i - 1][j] == 1 || i == 0))
				{
					// straight ║   ///  ║═
					AssetPathName = TEXT(STRAIGHT_VERT);
				}
				else if (
					/*(j < mapWidth && Map_Cells[i][j + 1] == 1 || j == mapWidth) && (j > 0 && Map_Cells[i][j - 1] ==
										1 || j == 0) && */
					(i < mapHeight && Map_Cells[i + 1][j] != 1 && i > 0 &&
					 Map_Cells[i - 1][j] != 1))
				{
					// straight ═   ///  ║═
					AssetPathName = TEXT(STRAIGHT_HORIZ);
				}
				// AssetPathName = TEXT("/Script/Engine.StaticMesh'/Game/_Main/Voxels/path/path.path'");
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				AMyTile *Tile = GetWorld()->SpawnActor<AMyTile>(AMyTile::StaticClass(), SpawnParameters);
				UStaticMesh *Asset = Cast<UStaticMesh>(
					StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));
				Tiles_Cells[i].Add(Tile);
				Tiles_Cells[i][j]->Mesh_L1->SetStaticMesh(Asset);
				Tiles_Cells[i][j]->Mesh_L1->SetWorldScale3D(SIZER);
				Tiles_Cells[i][j]->SceneComponent->SetWorldLocation(FVector(
					i * TILE_SIDE_LEN, j * TILE_SIDE_LEN, 0 * TILE_SIDE_LEN / 2));
			}
			if (Map_Cells[i][j] == static_cast<int>(EMapEnum::Obstacle))
			{
				FString AssetPathName = TEXT(GRASS_T1);
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				AMyTile *Tile = GetWorld()->SpawnActor<AMyTile>(AMyTile::StaticClass(), SpawnParameters);
				UStaticMesh *Asset = Cast<UStaticMesh>(
					StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));

				Tiles_Cells[i].Add(Tile);
				Tiles_Cells[i][j]->Mesh_L1->SetStaticMesh(Asset);
				Tiles_Cells[i][j]->Mesh_L1->SetWorldScale3D(SIZER);
				Tiles_Cells[i][j]->SceneComponent->SetWorldLocation(FVector(
					i * TILE_SIDE_LEN, j * TILE_SIDE_LEN, 0 * TILE_SIDE_LEN / 2));

				FNeighborFences NeighborFences;

				if (i == 0)
				{
					if (j == 0)
					{
						if (Map_Cells[i + 1][j] == -1)
						{
							NeighborFences.Up = true;
						}
						if (Map_Cells[i][j + 1] == -1)
						{
							NeighborFences.Right = true;
						}
					}
					else if (j == mapWidth - 1)
					{
						if (Map_Cells[i + 1][j] == -1)
						{
							NeighborFences.Up = true;
						}
						if (Map_Cells[i][j - 1] == -1)
						{
							NeighborFences.Left = true;
						}
					}
					else if (j > 0 && j < mapWidth - 1)
					{
						if (Map_Cells[i + 1][j] == -1)
						{
							NeighborFences.Up = true;
						}
						if (Map_Cells[i][j - 1] == -1)
						{
							NeighborFences.Left = true;
						}
						if (Map_Cells[i][j + 1] == -1)
						{
							NeighborFences.Right = true;
						}
					}
				}
				else if (i == mapHeight - 1)
				{
					if (j == 0)
					{
						if (Map_Cells[i - 1][j] == -1)
						{
							NeighborFences.Down = true;
						}
						if (Map_Cells[i][j + 1] == -1)
						{
							NeighborFences.Right = true;
						}
					}
					else if (j == mapWidth - 1)
					{
						if (Map_Cells[i - 1][j] == -1)
						{
							NeighborFences.Down = true;
						}
						if (Map_Cells[i][j - 1] == -1)
						{
							NeighborFences.Left = true;
						}
					}
					else if (j > 0 && j < mapWidth - 1)
					{
						if (Map_Cells[i - 1][j] == -1)
						{
							NeighborFences.Down = true;
						}
						if (Map_Cells[i][j - 1] == -1)
						{
							NeighborFences.Left = true;
						}
						if (Map_Cells[i][j + 1] == -1)
						{
							NeighborFences.Right = true;
						}
					}
				}
				else if (i > 0 && i < mapWidth - 1)
				{
					if (j == 0)
					{
						if (Map_Cells[i + 1][j] == -1)
						{
							NeighborFences.Up = true;
						}
						if (Map_Cells[i - 1][j] == -1)
						{
							NeighborFences.Down = true;
						}
						if (Map_Cells[i][j + 1] == -1)
						{
							NeighborFences.Right = true;
						}
					}
					else if (j == mapWidth - 1)
					{
						if (Map_Cells[i + 1][j] == -1)
						{
							NeighborFences.Up = true;
						}
						if (Map_Cells[i - 1][j] == -1)
						{
							NeighborFences.Down = true;
						}
						if (Map_Cells[i][j - 1] == -1)
						{
							NeighborFences.Left = true;
						}
					}
					else if (j > 0 && j < mapWidth - 1)
					{
						if (Map_Cells[i + 1][j] == -1)
						{
							NeighborFences.Up = true;
						}
						if (Map_Cells[i][j - 1] == -1)
						{
							NeighborFences.Left = true;
						}
						if (Map_Cells[i - 1][j] == -1)
						{
							NeighborFences.Down = true;
						}
						if (Map_Cells[i][j + 1] == -1)
						{
							NeighborFences.Right = true;
						}
					}
				}
				FString Fence_AssetPathName;
				FRotator Fence_Rotator;
				bool A = NeighborFences.Up;
				bool B = NeighborFences.Down;
				bool C = NeighborFences.Left;
				bool D = NeighborFences.Right;
				if (A && B && C && D || A && B && (!C != !D) || C && D && (A != B))
				{
					Fence_AssetPathName = TEXT(FENCE_ALLSIDE);
				}
				else
				{
					if (A && B)
					{
						Fence_AssetPathName = TEXT(FENCE_OPP);
						Fence_Rotator = FRotator(0, 90, 0);
					}
					else if (C && D)
					{
						Fence_AssetPathName = TEXT(FENCE_OPP);
						Fence_Rotator = FRotator(0, 0, 0);
					}
					else if (A && C)
					{
						Fence_AssetPathName = TEXT(FENCE_ANGULAR);
						Fence_Rotator = FRotator(0, 90, 0);
					}
					else if (A && D)
					{
						Fence_AssetPathName = TEXT(FENCE_ANGULAR);
						Fence_Rotator = FRotator(0, 180, 0);
					}
					else if (B && C)
					{
						Fence_AssetPathName = TEXT(FENCE_ANGULAR);
						Fence_Rotator = FRotator(0, 0, 0);
					}
					else if (B && D)
					{
						Fence_AssetPathName = TEXT(FENCE_ANGULAR);
						Fence_Rotator = FRotator(0, -90, 0);
					}
					else
					{
						if (A)
						{
							Fence_AssetPathName = TEXT(FENCE_SNGL);
							Fence_Rotator = FRotator(0, 90, 0);
						}
						else if (B)
						{
							Fence_AssetPathName = TEXT(FENCE_SNGL);
							Fence_Rotator = FRotator(0, -90, 0);
						}
						else if (C)
						{
							Fence_AssetPathName = TEXT(FENCE_SNGL);
							Fence_Rotator = FRotator(0, 0, 0);
						}
						else if (D)
						{
							Fence_AssetPathName = TEXT(FENCE_SNGL);
							Fence_Rotator = FRotator(0, 180, 0);
						}
						else
						{
							Fence_AssetPathName = TEXT(FENCE_ZERO);
						}
					}
				}
				FActorSpawnParameters SpawnParameters_ = FActorSpawnParameters();
				SpawnParameters.Owner = this;

				UStaticMesh *Asset_ = Cast<UStaticMesh>(
					StaticLoadObject(UStaticMesh::StaticClass(), this, *Fence_AssetPathName));

				Tiles_Cells[i][j]->Mesh_L2->SetStaticMesh(Asset_);
				Tiles_Cells[i][j]->Mesh_L2->SetWorldScale3D(SIZER);
				Tiles_Cells[i][j]->Mesh_L2->SetWorldRotation(Fence_Rotator);
				Tiles_Cells[i][j]->Mesh_L2->SetWorldLocation(
					Tiles_Cells[i][j]->SceneComponent->GetComponentLocation() + FVector(0, 0, 12));
			}
		}
	}

	/*bool a = GetWorld()->IsNavigationRebuilt();
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT("%i"), a));*/
	//UpdateNavMeshik();
}

void AMyMap::UpdateNavMeshik()
{
	// Получаем ссылку на навигационную систему
	UNavigationSystemV1 *NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(FAITestHelpers::GetWorld());
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

	MAP_FILE_NAME = "X:/EpicGames_Unreal/pROJECTs/Oparush/OparushDefense/Source/Game/Beta_map.bmp";
	bitmap_image Map(MAP_FILE_NAME);
	if (!Map)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("FILE DID NOT LOAD((")));

		return;
	}
	this->mapHeight = Map.height();
	this->mapWidth = Map.width();

	Map_Cells = new int *[this->mapHeight];
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

			if (R > 230 && G > 230 && B < 50)
			{
				StartCell.X_coord = j;
				StartCell.Y_coord = i;
			}

			if (R > 230 && G > 230 && B > 230)
			{
				Map_Cells[i][j] = static_cast<int>(EMapEnum::Spawner_two);
			}
			else if (R > 230 && G < 50 && B > 230)
			{
				Map_Cells[i][j] = static_cast<int>(EMapEnum::Spawner_one);
			}
			else if (R == 0 && G > 230 && B > 230)
			{
				Map_Cells[i][j] = static_cast<int>(EMapEnum::Nest);
			}
			else if (R > 230)
			{
				Map_Cells[i][j] = static_cast<int>(EMapEnum::Path); /// RED
			}
			else
			{
				if (G > 230)
				{
					Map_Cells[i][j] = static_cast<int>(EMapEnum::Field); /// GREEN
				}
				else
				{
					if (B > 230)
					{
						Map_Cells[i][j] = static_cast<int>(EMapEnum::Obstacle); /// BLUE
					}
					else
					{
						Map_Cells[i][j] = static_cast<int>(EMapEnum::Error); /// error pixel
					}
				}
			}
		}
	}
}

void AMyMap::ProcessWaypoints()
{
	// Nodes_Pre = new ANodePoint[mapHeight*mapWidth];
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

				// ANodePoint* Point = GetWorld()->SpawnActor<ANodePoint>(ANodePoint::StaticClass(), SpawnParameters);

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
			//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow,
											//  FString::Printf(
											// 	 TEXT("x: %i, y%i"), EndCell.X_coord, EndCell.Y_coord));
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

			// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue,
			// 								 FString::Printf(
			// 									 TEXT("x: %i, y%i"), EndCell.X_coord, EndCell.Y_coord));
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
			// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red,
			// 								 FString::Printf(
			// 									 TEXT("x: %i, y%i"), EndCell.X_coord, EndCell.Y_coord));
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

			// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White,
			// 								 FString::Printf(
			// 									 TEXT("x: %i, y%i"), EndCell.X_coord, EndCell.Y_coord));
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

		AWaypoint *Waypoint = GetWorld()->SpawnActor<AWaypoint>(
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
		UStaticMesh *Asset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));

		// Tiles_Cells[i].Add(Tile);

		// Tiles_Cells[i][j]->Mesh->SetStaticMesh(Asset);
		// Tiles_Cells[i][j]->Mesh->SetWorldScale3D(SIZER);
		// Tiles_Cells[i][j]->SceneComponent->SetWorldLocation(FVector( i * TILE_SIDE_LEN, j * TILE_SIDE_LEN, -200));
	}
	if (action == "DESTROYED")
	{
		this->Map_Cells[i][j] = 0;

		FString AssetPathName = TEXT(GRASS_T1);
		UStaticMesh *Asset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));

		// Tiles_Cells[i].Add(Tile);

		// Tiles_Cells[i][j]->Mesh->SetStaticMesh(Asset);
		// Tiles_Cells[i][j]->Mesh->SetWorldScale3D(SIZER);
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
