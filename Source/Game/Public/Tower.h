// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"

#include "Tower.generated.h"


UCLASS()
class GAME_API ATower : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATower();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	float GetCost() const;
	float GetDamage() const;
	float GetTowerLevel() const;
	
	float GetHealth() const;
	void SetHealth(float Input);
	
	UFUNCTION(BlueprintCallable, Category = "Tower|TakeDamage")
    virtual void Damage(float Input);
	
    UFUNCTION(BlueprintCallable, Category = "Tower|IncreaseHealthPoints")
    virtual void Heal(float Input);

	UFUNCTION(BlueprintCallable, Category = "Tower|BlastBullets")
    virtual void Blast();
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|HP")
	float HealthPoints = 10.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Cost")
	float PlacingCost = 10.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Level")
	float TowerLevel = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Damage")
	float DamageAmount = 2.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Range")
	double Range = 600.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Delay")
	double BlastDelay;
	
	UPROPERTY()
	TArray<AActor*> Enemies;

	// -----------------------------   setup
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereCollision;

public:
	

	UFUNCTION(BlueprintCallable, Category = "Tower|Place")
	ATower* Place();
	
	
	UFUNCTION(BlueprintCallable, Category = "Tower|Overlap")
	void CheckOverlap();
	UFUNCTION(BlueprintCallable, Category = "Tower|Collision|Overlap")
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "Tower|Collision|Overlap")
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool SuperClassIsEnemy(const UClass* Input);
	
	UPROPERTY()
	bool Placed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Damage")
    bool bIsSpawner;
    
	
};
