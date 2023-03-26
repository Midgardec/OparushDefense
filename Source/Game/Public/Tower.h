// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Tower|Place")
	ATower* Place();
	//ATower Clone() const;
	UPROPERTY()
	bool Placed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Damage")
    bool bIsSpawner;
    
	
};
