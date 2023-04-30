// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIController.h"
#include "MyCharacter.h"
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
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatedBy,
		AActor* DamageCauser
	);
	void MoveBetweenPoints(float DeltaTime);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveToWaypoint();
	AMyCharacter* GetPlayer() const;
	auto Place() -> AEnemyBase*;

	UPROPERTY()
	AMyCharacter* MyCharacter;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 500.f;
	bool canMove;
	bool Moving = false;


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int CurrentCheckPoint = 0;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	AMyMap* Map;

	int TILE_SIDE_LEN = 200;


	UPROPERTY()
	AEnemyAIController* EnemyAIController;


	// Переменная для хранения текущего направления спрайта
	EDirection CurrentDirection = EDirection::Right;

	// Задаем список спрайтов для каждого направления
	UPROPERTY(EditAnywhere, Category = "Sprite")
	TMap<EDirection, UPaperSprite*> Sprites;

	// Компонент спрайта
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* SpriteComponent;

	// Используемый спрайт
	UPROPERTY(BlueprintReadWrite, Category = "Sprite", meta = (AllowPrivateAccess = "true"))
	UPaperSprite* CurrentSprite;

	// SphereColision
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereCollision;
	

	UFUNCTION(BlueprintCallable)
	void SetHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	float GetHealth();

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float Amount);
	
	UFUNCTION(BlueprintCallable)
	float GetReward();
private:
	FVector TargetLocation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> Waypoints;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	float Reward;
};
