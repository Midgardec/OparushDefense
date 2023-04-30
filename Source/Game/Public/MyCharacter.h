// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tower.h"
#include "MyMap.h"
#include "ActorFactories/ActorFactoryTriggerBox.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"


UENUM(BlueprintType, meta = (UseEnumValuesAsMaskValuesInEditor="true"))
enum class EDirection : uint8
{
	Up UMETA(DisplayName = "Up"),
	Down UMETA(DisplayName = "Down"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};


class UCameraComponent;
class USpringArmComponent;
UCLASS()
class GAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void ChangeSprite(EDirection NewDirection);

protected:
	UFUNCTION()
	virtual void MoveVert(float AxisValue);
	UFUNCTION()
	virtual void MoveHoriz(float AxisValue);
	UFUNCTION()
	virtual void Zoom(float AxisValue);
	UFUNCTION(BlueprintCallable, Category = "Tower|Place")
	virtual void PlaceTower();
	
	UFUNCTION(BlueprintCallable, Category = "Tower|Spawn")
	virtual void SpawnTower(float DeltaTime);
	//virtual void SpawnTower(ATower *Tower);
	UFUNCTION(BlueprintCallable, Category = "Tower|Change")
	virtual void ChangeTower();
	UFUNCTION(BlueprintCallable, Category = "Player|Collision|Overlap")
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "Player|Collision|Overlap")
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Collision;
	
	FVector Direction;
	bool bReadyForWave = false;

private:

	//------------------------------------------------------ old character's setup
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;


	//------------------------------------------------------ new character's setup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> MyCapsuleComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> MyMesh;

	/*UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCineCameraComponent* CineCameraComponent;
	*/

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UPostProcessComponent* PostProcessComponent;
	*/
	
	// ---- sprite setup vars
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
	

	
	UPROPERTY()
	FVector TargetLocation;
	
	UPROPERTY()
	float TargetZoom;
	
	UPROPERTY()
	FRotator TargetRotation;


	// tower's vars
	UPROPERTY()
	bool Overlapped;
	
	UPROPERTY()
	ATower* OverlappedTower;
	
	UPROPERTY()
	ATower* TowerMember;

	UPROPERTY()
	FVector TowerMemberLocationShift;
	
	UPROPERTY()
	FVector TowerMemberLocation;

	UPROPERTY()
	bool bMoveVert;
	
	UPROPERTY()
	bool bMoveHoriz;

	UPROPERTY()
	bool bCanPlace;


	
	
protected:
	/// ------------------------
	/// vars for setting camera
	/// ------------------------
	///
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player's Vars|Coins")
	float coinsAmount = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player's Vars|Nuts")
	float nutsAmount = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|MoveSpeed")
	float MoveSpeed = 1500.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Zoom|Speed")
	float ZoomSpeed = 2.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|RotateSpeed")
	float RotateSpeed = 2.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Zoom|Min")
	float ZoomMin = 1500.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Zoom|Max")
	float ZoomMax = 4000.0f;
	
	UPROPERTY(BlueprintReadOnly)
	ATower *Tower;
public:
	
	UPROPERTY(BlueprintReadWrite)
	AMyMap *Map;
	
	const std::string PLACED = "PLACED";
	const std::string DESTROYED = "DESTROYED";
private:
	void CheckJump();

	UPROPERTY()
	bool Jumping;
};
