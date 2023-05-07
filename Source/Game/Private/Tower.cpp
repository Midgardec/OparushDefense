// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

#include "EnemyBase.h"
#include "TargetTester.h"
#include "Game/ProjectileBase.h"

#include "Kismet/GameplayStatics.h"

// Sets default values

ATower::ATower()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSphereRadius(BuyRange); // задаем радиус сферы
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ATower::OnOverlapBegin);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ATower::OnOverlapEnd);
	SphereCollision->SetCollisionProfileName(TEXT("Trigger")); // задаем профиль коллизии для сферы
}


// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();
	bIsSpawner = true;
	Placed = false;
	EnemyToShoot = nullptr;

	FString AssetPathName = TEXT("/Script/Engine.StaticMesh'/Game/_Main/tiles/obstacle_block.obstacle_block'");
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Owner = this;

	TargetTester = GetWorld()->SpawnActor<ATargetTester>(ATargetTester::StaticClass(), SpawnParameters);
	UStaticMesh* Asset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));

	TargetTester->Mesh->SetStaticMesh(Asset);
	TargetTester->SceneComponent->SetWorldLocation(FVector(0, 0, -200.f));


	GetWorldTimerManager().SetTimer(TimerHandler, this, &ATower::Blast, BlastDelay, true);

	SetDamage(20.f);
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsSpawner && Placed)
	{
		SphereCollision->SetSphereRadius(BlastRange);
		float minDistance = 10000000000.f;
		UGameplayStatics::GetAllActorsOfClass(this, AEnemyBase::StaticClass(), Enemies);

		

		for (auto Enemy : Enemies)
		{
			const float Distance = FVector::Distance(GetActorLocation(), Enemy->GetActorLocation());

			if (Distance <= minDistance)
			{
				minDistance = Distance;
				EnemyToShoot = Enemy;
			}
		}
		if (Enemies.Num() == 0) { bIsAllowedToShoot = false; }

		if (IsValid(EnemyToShoot)) //->IsPendingKill())
		{
			const float DistanceToEnemy = FVector::Distance(GetActorLocation(), EnemyToShoot->GetActorLocation());
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,FString::Printf(TEXT("%f"), DistanceToEnemy));

			if (DistanceToEnemy <= BlastRange)
			{
				bIsAllowedToShoot = true;
			}
			else
			{
				if (bIsAllowedToShoot)
				{
					EnemyToShoot_index = (EnemyToShoot_index + 1) % Enemies.Num();
				}
				bIsAllowedToShoot = false;
			}
		}
	}
}

// Called to bind functionality to input
void ATower::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ATower::GetCost() const
{
	return PlacingCost;
}

float ATower::GetDamage() const
{
	return DamageAmount;
}

void ATower::SetDamage(float Amount)
{
	DamageAmount = Amount;
}

float ATower::GetTowerLevel() const
{
	return TowerLevel;
}

float ATower::GetHealth() const
{
	return HealthPoints;
}

void ATower::SetHealth(const float Input)
{
	this->HealthPoints = Input;
}


void ATower::Damage(const float Input)
{
	this->HealthPoints -= Input;
}

void ATower::Heal(const float Input)
{
	this->HealthPoints += Input;
}


void ATower::Blast()
{
	if (!bIsAllowedToShoot)
	{
		return;
	}

	/// TODO: Create a new projectile and set its properties


	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	ProjectileClass = AProjectileBase::StaticClass();
	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, GetActorLocation(),
	                                                                      GetActorRotation(), SpawnParams);


	float Damage_ = this->GetDamage();
	Projectile->SetDamage(Damage_);

	constexpr float ProjectileSpeed = 1000.f;
	Projectile->SetSpeed(ProjectileSpeed);

	Projectile->SetTarget(EnemyToShoot);

	Projectile->SetType(EType::AutoTarget);

	// Spawn the projectile at the muzzle.
	if (Projectile)
	{
		// Set the projectile's initial trajectory.
		FVector LaunchDirection = (EnemyToShoot->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		Projectile->SetVelocity(LaunchDirection);
	}
}

ATower* ATower::Place()
{
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.Template = this;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White,FString::Printf(TEXT("text")));
		return GetWorld()->SpawnActor<ATower>(this->GetClass(), SpawnParameters);
	}

	return nullptr;
}

bool ATower::SuperClassIsEnemy(const UClass* Input)
{
	return Input->IsChildOf(AEnemyBase::StaticClass());
}


void ATower::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                            const FHitResult& SweepResult)
{
}

void ATower::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                          int32 OtherBodyIndex)
{
}
