// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetTester.h"

// Sets default values
ATargetTester::ATargetTester()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh0"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATargetTester::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

