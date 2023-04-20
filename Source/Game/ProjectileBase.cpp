// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"

#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a static mesh component
	UStaticMeshComponent* StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = StaticMeshComponent;

	// Create a projectile movement component
	UProjectileMovementComponent* ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		"ProjectileMovementComponent");
	ProjectileMovementComponent->SetUpdatedComponent(StaticMeshComponent);
	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	// Set default values for other properties
	Damage = 10.f;
}

void AProjectileBase::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}

void AProjectileBase::SetSpeed(float NewSpeed)
{
	Speed = NewSpeed;
	/*UProjectileMovementComponent* ProjectileMovementComponent = Cast<UProjectileMovementComponent>(
		GetComponentByClass(UProjectileMovementComponent::StaticClass()));
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->InitialSpeed = Speed;
		ProjectileMovementComponent->MaxSpeed = Speed;
	}*/
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	// Bind the OnHit function to the OnActorHit event
	
	//OnActorHit.AddDynamic(this, &AProjectileBase::OnHit);
}

void AProjectileBase::OnHit(AActor* TargetActor)
{
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileBase::SetTarget(AActor* NewTarget)
{
	Target = NewTarget;
}
