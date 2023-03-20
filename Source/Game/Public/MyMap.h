// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MyTile.h"
#include "CoreMinimal.h"
#include "NodePoint.h"
#include "GameFramework/Actor.h"
#include "MyMap.generated.h"



USTRUCT()
struct FTiles2DArray{
	GENERATED_BODY()
public:

	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObjectsInContainer
	TArray<AMyTile*> Ar;

	AMyTile* operator[] (int32 i) {
		return Ar[i];
	}

	void Add(AMyTile* tile) {
		Ar.Add(tile);
	}
};

USTRUCT()
struct FCoords
{
	GENERATED_BODY()
public:
	int X_coord;
	int Y_coord;
	
};

UCLASS()
class GAME_API AMyMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyMap();
	UFUNCTION(BlueprintCallable, Category = "Tiles|Size")
	void SetTileSize(int T_Size);
	UFUNCTION(BlueprintCallable, Category = "Map|Spawn")
	void SpawnMap();
	void UpdateNavMeshik();
	UFUNCTION(BlueprintCallable, Category = "Map|Convert from image")
	void ConvertMapFromImage();
	UFUNCTION(BlueprintCallable, Category = "Map|Waypoints")
	void ProcessWaypoints();
	UFUNCTION(BlueprintCallable, Category = "Map|Waypoints|Spawn")
	void SpawnWaypoints();
	void MapChange(int i, int j, std::string action);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	int mapWidth;
	UPROPERTY(BlueprintReadOnly)
	int mapHeight;
	/**
	 *  Map_Cells - 2d array of map's tile's information
	 *  where Map_Cells[i][j] - [i][j] tile's info
	 *
	 *  [][] == -1   =>  tile for "trees and bushes"		/ not for placement
	 *  [][] ==  1   =>  tile for "enemies path"			/ not for placement
	 *  [][] ==  2   =>  tile for "tower already placed"	/ not for placement
	 *  [][] ==  0   =>  tile for "tower to place"			/ for placement ☺
	 */
	int ** Map_Cells;
	const char * MAP_FILE_NAME ;
	//AMyTile * Tile; 
	
	UPROPERTY(EditAnywhere)
	TArray<FTiles2DArray> Tiles_Cells;
	int TILE_SIDE_LEN;
	UPROPERTY(EditAnywhere)
	class UInstancedStaticMeshComponent* Tiles;
	
	
	UPROPERTY(EditAnywhere)
	// ReSharper disable once UnrealHeaderToolError
	TArray<ANodePoint*> Nodes ;
	UPROPERTY(EditAnywhere)
	TArray<ANodePoint*> Nodes_Pre;

	UPROPERTY(EditAnywhere)
	FCoords StartCell;
	UPROPERTY(EditAnywhere)
	FCoords EndCell;
	UPROPERTY(EditAnywhere)
	int Nodes_Num;
};