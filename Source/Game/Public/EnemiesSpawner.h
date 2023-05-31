// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "EnemiesSpawner.generated.h"

UCLASS()
class GAME_API AEnemiesSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemiesSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void SpawnEnemy(UClass* EnemyClassToSpawn);
	void StartWave();
	void SpawnEnemies();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	AMyCharacter* GetPlayer() const;

	FTimerHandle TimerHandler;
	int WaveIndex = 1;
	UPROPERTY()
	AMyCharacter* Player;
	float SpawnDelay;

	int EnemyPerWaveCounter = 0;
};
