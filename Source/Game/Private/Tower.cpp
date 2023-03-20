// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

// Sets default values

ATower::ATower()
{
	PrimaryActorTick.bCanEverTick = true;
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
	
	if(!bIsSpawner)
	{
		Blast();
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

float ATower::GetLevel() const
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
	//____________
	//spawn bullets
	//____________
}

auto ATower::Place() -> ATower*
{
		
		if(GetWorld())
		{
			FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
            SpawnParameters.Template = this;
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White,FString::Printf(TEXT("text")));
			return GetWorld()->SpawnActor<ATower>(this->GetClass(), SpawnParameters);
			
			
            /*	
            
            f->SetActorLocation(Location);
			f->SetActorRotation(Rotation) ;
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
									 FString::Printf(TEXT("%f -x, %f -y"),f->GetActorLocation().X, f->GetActorLocation().Y));
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue,
											 FString::Printf(TEXT("%f -x, %f -y"),Location.X,Location.Y));
	
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Placed"));*/
		}

        return nullptr;
	
}

//ATower ATower::Clone() const {return NewObject<ATower>(ATower(this));}




