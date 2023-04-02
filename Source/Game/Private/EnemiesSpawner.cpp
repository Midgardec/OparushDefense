
// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemiesSpawner.h"

#include "EnemyBase.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemiesSpawner::AEnemiesSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemiesSpawner::BeginPlay()
{
	Super::BeginPlay();
	Player = GetPlayer();
}

void AEnemiesSpawner::SpawnEnemy()
{
	
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();

	GetWorld()->SpawnActor<AEnemyBase>(AEnemyBase::StaticClass(), SpawnParameters);
}

void AEnemiesSpawner::StartWave()
{
	
	//SpawnParameters.Template = this;
	int waveIterator = 1;
	WaveIndex++;
	SpawnDelay = 2.f;
	while(waveIterator < WaveIndex)
	{
		GEngine->AddOnScreenDebugMessage(-1,15.f, FColor::Cyan, FString::Printf(TEXT("Wave %i"), WaveIndex));
		waveIterator++;
		SpawnEnemy();
		GetWorldTimerManager().SetTimer(TimerHandler, this, &AEnemiesSpawner::SpawnEnemy, SpawnDelay, false);
	}
	
}

// Called every frame
void AEnemiesSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(Player)
	{
		if(Player->bReadyForWave)
		{
			StartWave();
			Player->bReadyForWave = false;
		}
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1,15.f, FColor::Turquoise, FString::Printf(TEXT("PUK puk oblomchik")));
	}
}

AMyCharacter* AEnemiesSpawner::GetPlayer() const 
{
	
	//ACharacter* TheCharacter = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetCharacter();
	if(GetWorld())
	{
		AMyCharacter* Mallet = Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)); //returns nullptr if failed
		if(!Mallet)
		{
			return nullptr;
		}
		
		return Mallet;
		
	}
	return nullptr;
}