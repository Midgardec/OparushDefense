// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CoinBase.generated.h"

UCLASS()
class GAME_API ACoinBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoinBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float Cost;
	float LifeTime;
	float Speed; // initial speed of the coin
	float LifeSpan;
	float MinHeight;
	float BounceCoefficient;
	int BounceCounter;
	int MaxBounces;
	float negAccel = 0.5;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	float GetCost();
	
	UFUNCTION()
	void Move(float time);

	// Set the speed of the projectile
	UFUNCTION(BlueprintCallable)
	void SetSpeed(float NewSpeed);

	// Function that initializes the projectile's velocity in the shoot direction.
	void SetVelocity(const FVector &Vector);
	void AddForce(FVector &NewForce);
	/// setup projectile component
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent *ProjectileMovementComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent *Bullet;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent *Collision;
};
