// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}


	///
	///	Collision setup
	/// 
	if (!Collision)
	{
		// Use a sphere as a simple collision representation.
		Collision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision radius.
		Collision->InitSphereRadius(100.0f);
		// Set the root component to be the collision component.
		RootComponent = Collision;

		
		
		Collision->SetCollisionProfileName(TEXT("Trigger"));
	}


	// Set default values for other properties
	Damage = 10.f;
	Speed = 100.f;
	///
	/// Create a projectile movement component
	///
	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
			TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(Collision);
		ProjectileMovementComponent->InitialSpeed = Speed;
		ProjectileMovementComponent->MaxSpeed = Speed;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}


	///
	/// //TODO: Bullet setup
	///
	if (!Bullet)
	{
		Bullet = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
		FString AssetPathName = TEXT("/Script/Engine.StaticMesh'/Game/_Main/tiles/obstacle_block.obstacle_block'");
		UStaticMesh* Asset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(
			TEXT("/Script/Engine.StaticMesh'/Engine/EditorMeshes/EditorSphere.EditorSphere'"));

		Bullet->SetStaticMesh(Asset);
		Bullet->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
		Bullet->SetupAttachment(RootComponent);
	}

	// Delete the projectile after N seconds.
	InitialLifeSpan = 4.0f;
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
	TSubclassOf<UDamageType> DamageType;
	UGameplayStatics::ApplyDamage(TargetActor, Damage, GetOwner()->GetInstigatorController(), this, DamageType);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,FString::Printf(TEXT("HitDetected: %s"), *TargetActor->GetName()));
	Destroy();
}


// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Type == EType::AutoTarget){
		if (IsValid(Target))
		{
			SetVelocity((Target->GetActorLocation() - this->GetActorLocation()).GetSafeNormal());
			if(FVector::Distance(GetActorLocation(), Target->GetActorLocation())<= 10.f)
			{
				OnHit(Target);
			}
		}
	}
		
}

void AProjectileBase::SetTarget(AActor* NewTarget)
{
	Target = NewTarget;
}

void AProjectileBase::SetType(EType NewType)
{
	Type = NewType;
}
void AProjectileBase::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}

void AProjectileBase::SetSpeed(float NewSpeed)
{
	Speed = NewSpeed;

	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
}

void AProjectileBase::SetVelocity(const FVector& Velocity)
{
	ProjectileMovementComponent->Velocity = Velocity * Speed;
}


