// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "TargetTester.h"
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
	void SetDamage(float Amount);
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
	float PlacingCost = 80.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Level")
	float TowerLevel = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Damage")
	float DamageAmount = 2.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Range")
	double BlastRange = 600.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Range")
	float BuyRange = 200.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Delay")
	double BlastDelay = 1.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|BlastCounter")
	int BlastCounter = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Max Blasts")
	int BlastCounterMax = 10;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Bullet speed")
	float BulletSpeed = 1000.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Bullet Type")
	EType BulletType = EType::AutoTarget;
	UPROPERTY()
	int posX;
	UPROPERTY()
	int posY;
	

	UFUNCTION(BlueprintCallable, Category = "Tower|Death")
	void Die();

	UPROPERTY()
	TArray<AActor*> Enemies_Extra;

	// -----------------------------   setup
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereCollision;
	FTimerHandle TimerHandler;

	UPROPERTY()
	ATargetTester* TargetTester;
	
	
public:
	
	UFUNCTION()
	void SetPosXY(int x, int y);


	UFUNCTION(BlueprintCallable, Category = "Tower|Place")
	ATower* Place();
	
	UFUNCTION(BlueprintCallable, Category = "Tower|Collision|Overlap")
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "Tower|Collision|Overlap")
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool SuperClassIsEnemy(const UClass* Input);
	
	UPROPERTY()
	bool Placed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Damage")
    bool bIsSpawner;
    
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|Damage")
	bool bIsAllowedToShoot= false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|bIsDead")
	bool isDead = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|bIsDead")
	bool blasted = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tower|bIsDead")
	bool bSpriteDirectionRight = false;

	UPROPERTY()
	TArray<AActor*> Enemies;

	UPROPERTY(BlueprintReadWrite,  EditAnywhere)
	AActor* EnemyToShoot;
	UPROPERTY()
	int EnemyToShoot_index = 0;

	// Projectile class to spawn.
	UPROPERTY(EditAnywhere, Category = Projectile)
	TSubclassOf<class AProjectileBase> ProjectileClass;
	


private:
	float timer_ = 0.0f;

};
