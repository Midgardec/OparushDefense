// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

#include "EnemyBase.h"

#include "Kismet/GameplayStatics.h"

// Sets default values

ATower::ATower()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSphereRadius(Range); // задаем радиус сферы
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ATower::OnOverlapBegin);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ATower::OnOverlapEnd);
	SphereCollision->SetCollisionProfileName(TEXT("Trigger"));// задаем профиль коллизии для сферы
}



// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();
	bIsSpawner = true;
	Placed = false;
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(!bIsSpawner && Placed)
	{
		AActor* Enemy = nullptr;
	
		if(Enemies.Num() > 0){Enemy = Enemies[0];}
	
		if (IsValid(Enemy))//->IsPendingKill())
		{
			const float DistanceToEnemy = FVector::Distance(GetActorLocation(), Enemy->GetActorLocation());
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,FString::Printf(TEXT("%f"), DistanceToEnemy));
													 
			if (DistanceToEnemy <= Range)
			{
				// Attack the enemy
				// ...
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White,TEXT("SHOOT"));
				
				this->Blast();
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
	/*TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), Enemies);
	*/

	
	
													 

			
			/// TODO: Create a new projectile and set its properties
			// AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, GetActorLocation(), GetActorRotation());
			// Projectile->SetDamage(DamageAmount);
			// Projectile->SetSpeed(ProjectileSpeed);

			// TODO: Calculate the direction to the enemy and set the projectile's velocity
			// FVector Direction = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			// Projectile->SetVelocity(Direction * ProjectileSpeed);

			// Set the projectile's lifespan and start moving it
			// Projectile->SetLifeSpan(ProjectileRange / ProjectileSpeed);
			// Projectile->StartMoving();
		
		/*else
		{
			Enemies.RemoveAtSwap(0, 1, false);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
													 FString::Printf(TEXT("deleted")));
		}*/
	
}

auto ATower::Place() -> ATower*
{
		
		if(GetWorld())
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
	
	/*UClass* SuperClass = Input;
	while (SuperClass->GetSuperClass())
	{
		
		if(SuperClass->GetName()=="EnemyBase")
		{
			return true;
		}
		SuperClass = SuperClass->GetSuperClass();
	}
	return false;*/
}


void ATower::CheckOverlap()
{
	
}


void ATower::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
															 
	if(OtherActor!=this)
	{
		
		/*TArray<AActor*> OverlappingActors;
		SphereCollision->GetOverlappingActors(OverlappingActors, AEnemyBase::StaticClass());*/
		
		if(SuperClassIsEnemy(OtherActor->GetClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue,FString::Printf(TEXT("text")));
			Enemies.Push(OtherActor);
		}
		
		
	}
	
	
	                                          	
}

void ATower::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if(OtherActor!=this)
	{
		if(SuperClassIsEnemy(OtherActor->GetClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,FString::Printf(TEXT("text")));
			Enemies.RemoveAtSwap(0, 1, false);
        }
	}
			
}