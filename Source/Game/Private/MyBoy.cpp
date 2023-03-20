// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBoy.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#define DefaultRotationPitch -45
#define TM_LOCATION_SHIFT 200
#define TILE_SIDE_LEN 200

// Sets default values
AMyBoy::AMyBoy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
///
///	SpringArm setup
/// 
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 2000.0f;
	SpringArmComponent->bDoCollisionTest = false;

///
///	Collision setup
/// 
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComponent"));
	Collision->SetupAttachment(RootComponent);
	Collision->SetBoxExtent(FVector(32.f,32.f,32.f));
	Collision->SetWorldScale3D(FVector(5.f,5.f,5.f));
	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AMyBoy::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AMyBoy::OnOverlapEnd);
	Collision->SetCollisionProfileName(TEXT("Trigger"));
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
}

// Called when the game starts or when spawned
void AMyBoy::BeginPlay()
{
	Super::BeginPlay();
	TargetLocation = GetActorLocation();
	TargetZoom = 2000.0f;

	
	const FRotator Rotation = SpringArmComponent->GetRelativeRotation();
	
	TargetRotation = FRotator(Rotation.Pitch + DefaultRotationPitch, Rotation.Yaw, 0.0f);

	coinsAmount = 10;
	nutsAmount = 10;
	
	///
	///ATower* TowerNew = NewObject<ATower>(); - создание объектов класса происходит данным образом
	///
	TowerMember = nullptr;
	Tower = nullptr;

	TowerMemberLocation.Z = 150;


	/// map construction
	/// 1 spawning
	/// 2 changing width and height
	const FVector MapLocation(0.0f, 0.0f, 0.0f);
	const FRotator MapRotation(0.0f, 0.0f, 0.0f);
	const FActorSpawnParameters SpawnInfo;
	Map = GetWorld()->SpawnActor<AMyMap>(MapLocation,MapRotation, SpawnInfo);


	Jumping = false;
    	if(CanJump())
    	{
    		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("JUMP((("));
    		
    	}
}

// Called every frame
void AMyBoy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	const FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, 0.1, MoveSpeed);
	SetActorLocation(InterpolatedLocation);
	
	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, ZoomSpeed);
	SpringArmComponent->TargetArmLength = InterpolatedZoom;

	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, RotateSpeed);
	SpringArmComponent->SetRelativeRotation(InterpolatedRotation);

	this->SpawnTower(DeltaTime);

	
	if(Jumping)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("JUMP((("));
		this->Jump();
		//this->StopJumping();
	}
}

// Called to bind functionality to input
void AMyBoy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("MoveVert"), this, &AMyBoy::MoveVert);
	PlayerInputComponent->BindAxis(TEXT("MoveHoriz"), this, &AMyBoy::MoveHoriz);
	PlayerInputComponent->BindAxis(TEXT("CamZoom"), this, &AMyBoy::Zoom);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyBoy::CheckJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyBoy::CheckJump);

	PlayerInputComponent->BindAction(TEXT("Action|Take"), IE_Pressed, this, &AMyBoy::ChangeTower);
	PlayerInputComponent->BindAction(TEXT("Action|Place"), IE_Pressed, this, &AMyBoy::PlaceTower);
	
}

void AMyBoy::CheckJump()
{
	if(Jumping)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("already jumping"));
		Jumping = false;
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("JUMPED"));
		Jumping = true;
	}
}

void AMyBoy::MoveVert(float AxisValue)
{
	if(AxisValue == 0.f){
		bMoveVert = false;
		return;
	}
	bMoveVert = true;
		
	TargetLocation = GetActorForwardVector() * GetWorld()->GetDeltaSeconds() * AxisValue * MoveSpeed + TargetLocation;

	if(AxisValue > 0){
		if(!bMoveHoriz)
		{
			TowerMemberLocationShift.Y = 0;
		}
		TowerMemberLocationShift.X = TM_LOCATION_SHIFT;
	}
	else{
		if(!bMoveHoriz)
		{
			TowerMemberLocationShift.Y = 0;
		}
		TowerMemberLocationShift.X = -TM_LOCATION_SHIFT;
	}
}

void AMyBoy::MoveHoriz(float AxisValue)
{
	if(AxisValue == 0.f){
		bMoveHoriz = false;
		return;
	}
	bMoveHoriz = true;
		
	TargetLocation = GetActorRightVector() * GetWorld()->GetDeltaSeconds() * AxisValue * MoveSpeed + TargetLocation;
	if(AxisValue > 0){
		if(!bMoveVert)
		{
			TowerMemberLocationShift.X =  0;
		}
		TowerMemberLocationShift.Y = TM_LOCATION_SHIFT;
	}
	else{
		if(!bMoveVert)
		{
			TowerMemberLocationShift.X = 0;
		}
		TowerMemberLocationShift.Y = -TM_LOCATION_SHIFT;
	}
}

void AMyBoy::Zoom(float AxisValue)
{
	if(AxisValue == 0.f)
	{
		return;
	}

	const float Zoom = AxisValue * 100.f;
	TargetZoom = FMath :: Clamp(Zoom + TargetZoom, ZoomMin, ZoomMax);
}

void AMyBoy::PlaceTower()
{
	if(TowerMember!=nullptr)
	{
		
		if(bCanPlace)
		{
			int X = FMath::Clamp(TowerMemberLocation.X/TILE_SIDE_LEN, 0, Map->mapHeight -1) ;
			int Y = FMath::Clamp(TowerMemberLocation.Y/TILE_SIDE_LEN, 0, Map->mapHeight -1) ;
			TowerMemberLocation.X = round((TowerMemberLocationShift.X + this->GetActorLocation().X) / TILE_SIDE_LEN) * TILE_SIDE_LEN;
			TowerMemberLocation.Y = round((TowerMemberLocationShift.Y + this->GetActorLocation().Y) / TILE_SIDE_LEN) * TILE_SIDE_LEN;
			TowerMemberLocation.X = FMath::Clamp(TowerMemberLocation.X, 0, TILE_SIDE_LEN * (Map->mapWidth-1));
			TowerMemberLocation.Y = FMath::Clamp(TowerMemberLocation.Y, 0, TILE_SIDE_LEN * (Map->mapHeight-1));

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("%i"),
				Map->Map_Cells[X][Y]));

			if(Map->Map_Cells[X][Y]==0)
			{
				TowerMember->SetActorLocation(TowerMemberLocation);
				TowerMember = nullptr;
				Map->Map_Cells[X][Y] = 2;
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("%i"),
				Map->Map_Cells[X][Y]));
			}else
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("ЗАНЯТО!!!"));
			}
			
			
		}
		
		
		
	}
}

void AMyBoy::SpawnTower(float DeltaTime)
{
	if(TowerMember != nullptr)
	{
		TowerMemberLocation.X = round((TowerMemberLocationShift.X + this->GetActorLocation().X) / TILE_SIDE_LEN) * TILE_SIDE_LEN;
		TowerMemberLocation.Y = round((TowerMemberLocationShift.Y + this->GetActorLocation().Y) / TILE_SIDE_LEN) * TILE_SIDE_LEN;

		TowerMemberLocation.X = FMath::Clamp(TowerMemberLocation.X, 0, TILE_SIDE_LEN * (Map->mapWidth -1));
		TowerMemberLocation.Y = FMath::Clamp(TowerMemberLocation.Y, 0, TILE_SIDE_LEN * (Map->mapHeight-1));

		const FVector Interpolated_Position = UKismetMathLibrary::VInterpTo(TowerMember->GetActorLocation(),
																			TowerMemberLocation,
																			DeltaTime, 15);

		TowerMember->SetActorLocation(Interpolated_Position);
		if( abs((TowerMember->GetActorLocation() - TowerMemberLocation).X) < 100 &&
			abs((TowerMember->GetActorLocation() - TowerMemberLocation).Y) < 100 &&
			abs((TowerMember->GetActorLocation() - TowerMemberLocation).Z) < 100 )
		{
			bCanPlace = true;
		}else
		{
			bCanPlace = false;
		}
	}
}

void AMyBoy::ChangeTower()
{
	if(Overlapped)
	{
		if (OverlappedTower!=nullptr)
		{
			if(OverlappedTower->bIsSpawner)
			{
				Tower = nullptr;
				
				
				Tower = OverlappedTower;
                			
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Statement Changed"));
			}
			
		}
	}



	if(this->Tower != nullptr && TowerMember == nullptr)
	{
		ATower* f = Tower->Place();
		
		TowerMember = f;
		TowerMember->bIsSpawner = false;
		this->Tower = nullptr;

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("end of ChangeTower()"));                      		
	}
}

void AMyBoy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor!=this)
	{
		Overlapped = true;
		OverlappedTower = CopyTemp(Cast<ATower>(OtherActor));
	}
}

void AMyBoy::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                          int32 OtherBodyIndex)
{
	Overlapped = false;
}
