// Fill out your copyright notice in the Description page of Project Settings. 

#pragma once

#include "CoreMinimal.h"
#include "Tower.h"
#include "MyMap.h"
#include "ActorFactories/ActorFactoryTriggerBox.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "MyBoy.generated.h"

class UCameraComponent;
class USpringArmComponent;
UCLASS()
class GAME_API AMyBoy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyBoy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	

private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Collision;

	UPROPERTY()
	FVector TargetLocation;
	
	UPROPERTY()
	float TargetZoom;
	
	UPROPERTY()
	FRotator TargetRotation;

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
	/// var's for setting camera
	/// ------------------------
	///
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player's Vars|Coins")
	float coinsAmount = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player's Vars|Nuts")
	float nutsAmount = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|MoveSpeed")
	float MoveSpeed = 1000.0f;
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

	UPROPERTY(BlueprintReadWrite)
	AMyMap *Map;
	
private:
	void CheckJump();

	UPROPERTY()
	bool Jumping;
};
