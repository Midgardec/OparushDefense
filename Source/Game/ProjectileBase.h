// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "ProjectileBase.generated.h"

UENUM()
enum class EType : uint8
{
	AutoTarget UMETA(DisplayName = "autoTarget"),
	ManualTarget UMETA(DisplayName = "manualTarget")
};

UCLASS()
class GAME_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the projectile hits its target
	virtual void OnHit(AActor* TargetActor);

	// The target actor for the projectile to track and hit
	UPROPERTY(BlueprintReadOnly)
	AActor* Target;

	// The amount of damage that the projectile does
	UPROPERTY(BlueprintReadOnly)
	float Damage;

	// The speed of the projectile
	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadWrite)
	EType Type;

public:
	/// setup the projectile
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	// Sphere collision component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* Collision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta =(AllowPrivateAccess = "true"))
	UStaticMeshComponent* Bullet;

	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	UMaterialInstanceDynamic* ProjectileMaterialInstance;

	// Function that initializes the projectile's velocity in the shoot direction.
	void SetVelocity(const FVector& Vector);

	// Set the amount of damage that the projectile does
	UFUNCTION(BlueprintCallable)
	void SetDamage(float NewDamage);

	// Set the speed of the projectile
	UFUNCTION(BlueprintCallable)
	void SetSpeed(float NewSpeed);

	// Set the target actor for the projectile to track and hit
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable)
	void SetType(EType NewType);

	
};
