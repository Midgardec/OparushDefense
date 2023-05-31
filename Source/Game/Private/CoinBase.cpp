// Fill out your copyright notice in the Description page of Project Settings.

#include "CoinBase.h"
#include "MyCharacter.h"
// Sets default values
ACoinBase::ACoinBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// SetActorEnableCollision(true);
	float GravityScale = 1.f;
	Cost = 10.f;
	Speed = 500.f;
	LifeSpan = 100.f;
	LifeTime = 0.f;
	MinHeight = 100.f;
	BounceCoefficient = -0.5f;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!Collision)
	{
		// Use a sphere as a simple collision representation.
		Collision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision radius.
		Collision->InitSphereRadius(0.01f);
		// Set the root component to be the collision component.
		RootComponent = Collision;

		Collision->SetCollisionProfileName(TEXT("Trigger"));
	}

	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
			TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(Collision);
		ProjectileMovementComponent->InitialSpeed = Speed;
		ProjectileMovementComponent->MaxSpeed = Speed;
		ProjectileMovementComponent->bRotationFollowsVelocity = false;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->ProjectileGravityScale = GravityScale;
	}

	if (!Bullet)
	{
		Bullet = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
		FString AssetPathName = TEXT("/Script/Engine.StaticMesh'/Game/_Main/tiles/obstacle_block.obstacle_block'");
		UStaticMesh *Asset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(
			TEXT("/Script/Engine.StaticMesh'/Engine/EditorMeshes/EditorSphere.EditorSphere'"));
		Bullet->SetStaticMesh(Asset);
		Bullet->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
		Bullet->SetupAttachment(RootComponent);
		Bullet->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

// Called when the game starts or when spawned
void ACoinBase::BeginPlay()
{
	Super::BeginPlay();
	BounceCounter = 0;
	MaxBounces = 3;
	this->SetLifeSpan(LifeSpan);
}

// Called every frame
void ACoinBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector CurrentLocation = GetActorLocation();
	ProjectileMovementComponent->Velocity.X *= negAccel;
	ProjectileMovementComponent->Velocity.Y *= negAccel;
	if (BounceCounter < MaxBounces)
	{
		if (CurrentLocation.Z < MinHeight)
		{
			CurrentLocation.Z = MinHeight;
			SetActorLocation(CurrentLocation);
			if (ProjectileMovementComponent->Velocity.Z < 0)
			{
				BounceCounter++;
				FVector NewVelocity = FVector(ProjectileMovementComponent->Velocity.X, ProjectileMovementComponent->Velocity.Y,
											  (ProjectileMovementComponent->Velocity.Z * BounceCoefficient));
				ProjectileMovementComponent->Velocity.Z = ProjectileMovementComponent->Velocity.Z * BounceCoefficient;
				// SetVelocity(NewVelocity);
			}
		}
	}
	else
	{
		
		ProjectileMovementComponent->Velocity.Z = 0;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
		// SetVelocity(FVector(ProjectileMovementComponent->Velocity.X, ProjectileMovementComponent->Velocity.Y, 0));
		// Move(LifeTime);
	}
	if(ProjectileMovementComponent->Velocity.X < 20 ){
		ProjectileMovementComponent->Velocity.X = 0;
	}
	
	if(ProjectileMovementComponent->Velocity.Y < 20 ){
		ProjectileMovementComponent->Velocity.Y = 0;
	}
	// Move(LifeTime);
}

void ACoinBase::Move(float time)
{
	this->SetActorLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, 200.f + 50 * FMath::Sin(time)));
}
void ACoinBase::AddForce(FVector &NewForce)
{
	ProjectileMovementComponent->AddForce(NewForce * Speed);
}
void ACoinBase::SetSpeed(float NewSpeed)
{
	this->Speed = NewSpeed;

	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
}

void ACoinBase::SetVelocity(const FVector &Velocity)
{
	ProjectileMovementComponent->Velocity = (Velocity * Speed);
}


float ACoinBase::GetCost()
{
	return Cost;
}