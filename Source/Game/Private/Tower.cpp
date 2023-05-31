// Fill out your copyright notice in the Description page of Project Settings.

#include "Tower.h"

#include "EnemyBase.h"

#include "TargetTester.h"

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
	UStaticMesh *Asset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), this, *AssetPathName));

	TargetTester->Mesh->SetStaticMesh(Asset);
	TargetTester->SceneComponent->SetWorldLocation(FVector(0, 0, -200.f));

	// GetWorldTimerManager().SetTimer(TimerHandler, this, &ATower::Blast, BlastDelay, true);

	// SetDamage(20.f);
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	timer_ += DeltaTime;
	if (!bIsSpawner && Placed)
	{

		float minDistance = 10000000.f;
		UGameplayStatics::GetAllActorsOfClass(this, AEnemyBase::StaticClass(), Enemies);

		this->EnemyToShoot = nullptr;
		if (Enemies.Num() == 0)
		{
			bIsAllowedToShoot = false;
		}
		else
		{

			for (auto Enemy : Enemies)
			{
				const float Distance = FVector::Distance(GetActorLocation(), Enemy->GetActorLocation());
				if (Cast<AEnemyBase>(Enemy)->GetHealth() < 0)
				{
					continue;
				}
				if (Distance < minDistance)
				{
					minDistance = Distance;
					this->EnemyToShoot = Enemy;
				}
			}

			if (this->EnemyToShoot)
			{
				const float DistanceToEnemy = FVector::Distance(GetActorLocation(), EnemyToShoot->GetActorLocation());
				if (DistanceToEnemy <= BlastRange)
				{
					bIsAllowedToShoot = true;
				}
				else
				{
					bIsAllowedToShoot = false;
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString::Printf(TEXT("invalid Enemy")));
				Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))->bWaveStart = false;

				bIsAllowedToShoot = false;
			}
		}
		if (timer_ >= BlastDelay)
		{
			timer_ = 0;
			Blast();
		}
		else
		{
			if (this->BlastCounter == 1)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("%i Enemies"), Enemies.Num()));
				timer_ = 0;
				Blast();
			}
		}
	}
}

// Called to bind functionality to input
void ATower::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
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
void ATower::Die()
{
	bIsAllowedToShoot = false;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Tower is dead")));
	isDead = true;
	Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))->Map->MapChange(posX, posY, "DESTROYED");
	this->SetLifeSpan(1.f);
}

void ATower::Blast()
{
	if (!bIsAllowedToShoot || isDead)
	{
		return;
	}
	blasted = true;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, FString::Printf(TEXT("BLAST %i"), BlastCounter));

	this->BlastCounter++;
	/// TODO: Create a new projectile and set its properties

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	ProjectileClass = AProjectileBase::StaticClass();
	AProjectileBase *Projectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, GetActorLocation(),
																		  GetActorRotation(), SpawnParams);

	float Damage_ = this->GetDamage();
	Projectile->SetDamage(Damage_);

	

	Projectile->SetTarget(EnemyToShoot);

	Projectile->SetType(BulletType);
	
	
	Projectile->SetSpeed(BulletSpeed);

	bSpriteDirectionRight = (EnemyToShoot->GetActorLocation().Y - GetActorLocation().Y )>=0;


	// Spawn the projectile at the muzzle.
	if (Projectile)
	{
		// Set the projectile's initial trajectory.
		FVector LaunchDirection = (EnemyToShoot->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		Projectile->SetVelocity(LaunchDirection);
	}
	
	if (this->BlastCounter > BlastCounterMax)
	{
		Die();
	}
}

ATower *ATower::Place()
{
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.Template = this;
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White,FString::Printf(TEXT("text")));
		return GetWorld()->SpawnActor<ATower>(this->GetClass(), SpawnParameters);
	}

	return nullptr;
}

bool ATower::SuperClassIsEnemy(const UClass *Input)
{
	return Input->IsChildOf(AEnemyBase::StaticClass());
}

void ATower::OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
							UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
							const FHitResult &SweepResult)
{
}

void ATower::OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp,
						  int32 OtherBodyIndex)
{
}

void ATower::SetPosXY(int x, int y)
{
	posX = x;
	posY = y;
}