// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Waypoint.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AWaypoint : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Player|Collision|Overlap")
	int GetWaypointOrder();
	
	void SetWaypoinOrder(int Indx);
private:
	UPROPERTY()
	int WaypointOrder;

	
};
