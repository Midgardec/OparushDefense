// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTile.h"
#include <Components/StaticMeshComponent.h>
// Sets default values
AMyTile::AMyTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh0"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

auto AMyTile::Place() -> AMyTile*
{
		
	if(GetWorld())
	{
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.Template = this;
		return GetWorld()->SpawnActor<AMyTile>(this->GetClass(), SpawnParameters);
	}

	return nullptr;
	
}
