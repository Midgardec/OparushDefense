// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIController.h"
#include "MyCharacter.h"
#include "Math/RandomStream.h"
#include "CoinBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.generated.h"

UCLASS()
class GAME_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void ChangeSprite(EDirection NewDirection);
	UFUNCTION()
	void OnAnyDamage(
		AActor *DamagedActor,
		float Damage,
		const UDamageType *DamageType,
		AController *InstigatedBy,
		AActor *DamageCauser);
	void MoveBetweenPoints(float DeltaTime);


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy|Hp")
	float Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy|MaxHp")
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy|Reward")
	float Reward;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	void MoveToWaypoint();
	AMyCharacter *GetPlayer() const;
	auto Place() -> AEnemyBase *;

	UPROPERTY()
	AMyCharacter *MyCharacter;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 500.f;
	bool canMove;
	bool isDead = false;
	bool Moving = false;
	bool isDamaging = false;
	bool CoinsSpawned = false;

	FTimerHandle TimerHandler;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy|Damage|Delay")
	float DamageDelay = 1.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy|Damage|DamageAmount")
	float DamageAmount = 10.f;
	UFUNCTION(BlueprintCallable)
	void DamageCastle();

	float timer_ = 0.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int CurrentCheckPoint = 0;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	AMyMap *Map;

	int TILE_SIDE_LEN = 200;

	UPROPERTY()
	AEnemyAIController *EnemyAIController;

	// Переменная для хранения текущего направления спрайта
	EDirection CurrentDirection = EDirection::Right;

	// Задаем список спрайтов для каждого направления
	UPROPERTY(EditAnywhere, Category = "Sprite")
	TMap<EDirection, UPaperFlipbook *> Sprites;

	// Компонент спрайта
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbookComponent *SpriteComponent;

	// Используемый спрайт
	UPROPERTY(BlueprintReadWrite, Category = "Flipbook", meta = (AllowPrivateAccess = "true"))
	UPaperFlipbook *CurrentSprite;

	// SphereColision
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USphereComponent *SphereCollision;

	UFUNCTION(BlueprintCallable)
	void AddDamage(float Damage);

	UFUNCTION(BlueprintCallable)
	float GetHealth();

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void SetReward(float Amount);

	UFUNCTION(BlueprintCallable)
	float GetReward();

	UFUNCTION(BlueprintCallable)
	void SpawnCoins();

	TSubclassOf<class ACoinBase> CoinClass;
	




private:
	UFUNCTION(BlueprintCallable)
	void SetHealth(float Amount);

	UFUNCTION()
	void Die(float TimeBeforeDie);

	FVector TargetLocation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent *SceneComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent *MeshComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<AActor *> Waypoints;

	
};
