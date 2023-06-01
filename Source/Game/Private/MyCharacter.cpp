// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "PaperSprite.h"
#include "CoinBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PaperFlipbook.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#define DefaultRotationPitch -30
#define DefaultRotationYaw -22.5
#define TM_LOCATION_SHIFT 100
#define TILE_SIDE_LEN 200
#define ARM_LENGTH_MIN 1000.f
#define ARM_LENGTH_MAX 20000.f
#define FOV 20
// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	///
	/// ----------------
	/// Set up Character's components
	/// ----------------
	///=================================================================@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	bReadyForWave = false;
	this->GetMesh()->SetupAttachment(RootComponent);
	///
	///	SpringArm setup
	///
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetMesh());

	SpringArmComponent->TargetArmLength = ARM_LENGTH_MIN;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->FieldOfView = FOV;

	///
	///	Collision setup
	///
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComponent"));
	Collision->SetupAttachment(RootComponent);
	Collision->SetBoxExtent(FVector(32.f, 32.f, 32.f));
	Collision->SetWorldScale3D(FVector(2.f, 2.f, 2.f));
	Collision->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AMyCharacter::OnOverlapEnd);
	Collision->SetCollisionProfileName(TEXT("Trigger"));

	this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	///
	/// Sprite setup
	///

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpriteComponent"));
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetupAttachment(RootComponent);
	FlipbookComponent->SetGenerateOverlapEvents(false);
	FlipbookComponent->SetRelativeLocation(FVector(0, 0, -80.f));
	FlipbookComponent->SetCastShadow(true);
	///
	/// Post Process Component
	///

	/*PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));

	PostProcessComponent->Settings.DepthOfFieldSensorWidth = 144.f;
	PostProcessComponent->Settings.DepthOfFieldFocalDistance = SpringArmComponent->TargetArmLength;
	PostProcessComponent->Settings.DepthOfFieldFstop = 0.8f;
	*/

	UPaperFlipbook *UpRun = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/player/flipbooks/RunBack.RunBack'"));
	UPaperFlipbook *DownRun = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/player/flipbooks/RunFront.RunFront'"));
	UPaperFlipbook *RightRun = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/player/flipbooks/RunRight.RunRight'"));

	UPaperFlipbook *UpIdle = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/player/flipbooks/IdleBack.IdleBack'"));
	UPaperFlipbook *DownIdle = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/player/flipbooks/IdleFront.IdleFront'"));
	UPaperFlipbook *RightIdle = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/player/flipbooks/Idle_right.Idle_right'"));

	SpritesRun.Add(EDirection::Up, UpRun);
	SpritesRun.Add(EDirection::Down, DownRun);
	SpritesRun.Add(EDirection::Left, RightRun);
	SpritesRun.Add(EDirection::Right, RightRun);

	SpritesIdle.Add(EDirection::Up, UpIdle);
	SpritesIdle.Add(EDirection::Down, DownIdle);
	SpritesIdle.Add(EDirection::Left, RightIdle);
	SpritesIdle.Add(EDirection::Right, RightIdle);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{

	Super::BeginPlay();

	TargetZoom = 2000.0f;

	CurrentCastleHealth = MaxCastleHealth;

	/*const FRotator Rotation = SpringArmComponent->GetRelativeRotation();
	 */

	const FRotator Rotation = SpringArmComponent->GetRelativeRotation();
	// TargetRotation = FRotator(DefaultRotationPitch, DefaultRotationYaw, 0);

	// SpringArmComponent->SetRelativeRotation(FRotator(DefaultRotationPitch, DefaultRotationYaw, 0));
	SpringArmComponent->SetWorldRotation(FRotator(DefaultRotationPitch, 0, 0));
	// SpringArmComponent->SetRelativeRotation(FRotator(DefaultRotationPitch, DefaultRotationYaw, 0));

	// TargetRotation = FRotator(Rotation.Pitch + DefaultRotationPitch, Rotation.Yaw + DefaultRotationYaw, Rotation.Roll);

	coinsAmount = 500;
	nutsAmount = 10;

	///
	/// ATower* TowerNew = NewObject<ATower>(); - создание объектов класса происходит данным образом
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
	Map = GetWorld()->SpawnActor<AMyMap>(MapLocation, MapRotation, SpawnInfo);
	if (Map)
	{
		Map->SetTileSize(TILE_SIDE_LEN);
		Map->ConvertMapFromImage();
		Map->SpawnMap();
		Map->ProcessWaypoints();
		this->SetActorLocation(FVector(Map->StartCell.Y_coord * TILE_SIDE_LEN, Map->StartCell.X_coord * TILE_SIDE_LEN, 0));
	}
	Jumping = false;

	// Устанавливаем начальный спрайт
	// CurrentSprite = ;

	CurrentSprite = SpritesIdle[EDirection::Up];
	FlipbookComponent->SetRelativeRotation(FRotator(0.0f, 90.f, 0.0f));
	FlipbookComponent->SetFlipbook(CurrentSprite);
	FlipbookComponent->SetWorldScale3D(FVector(5.0f, 5.0f, 5.0f));
	FlipbookComponent->SetCastShadow(true);
}
// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveVert"), this, &AMyCharacter::MoveVert);
	PlayerInputComponent->BindAxis(TEXT("MoveHoriz"), this, &AMyCharacter::MoveHoriz);
	PlayerInputComponent->BindAxis(TEXT("CamZoom"), this, &AMyCharacter::Zoom);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::CheckJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::CheckJump);

	PlayerInputComponent->BindAction("Ready", IE_Pressed, this, &AMyCharacter::SetAsReady);

	PlayerInputComponent->BindAction(TEXT("Action|Take"), IE_Pressed, this, &AMyCharacter::ChangeTower);
	PlayerInputComponent->BindAction(TEXT("Action|Place"), IE_Pressed, this, &AMyCharacter::PlaceTower);
}
void AMyCharacter::SetAsReady()
{
	if (bCanStartWave && !bWaveStart)
	{
		this->SetReady(true);
	}
}

void AMyCharacter::SetReady(bool state)
{
	this->bReadyForWave = state;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (!bWaveStart)
	{
		timer_ += DeltaTime;
	}
	else
	{
		timer_ = 0;
	}
	if (bCanChangeWave && !bWaveStart)
	{
		WaveIndex++;
		bCanChangeWave = false;
	}

	if (timer_ >= time_between_waves && !bWaveStart)
	{
		timer_ = 0;
		SetReady(true);
		bWaveStart = true;
	}

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%f"), GetCoins()));

	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, ZoomSpeed);
	this->SpringArmComponent->TargetArmLength = InterpolatedZoom;
	// PostProcessComponent->Settings.DepthOfFieldFocalDistance = SpringArmComponent->TargetArmLength;

	// const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, RotateSpeed);
	// SpringArmComponent->SetWorldRotation(TargetRotation);
	// SpringArmComponent->SetRelativeRotation(InterpolatedRotation);
	this->SpawnTower(DeltaTime);

	// Определяем новое направление спрайта в зависимости от направления движения

	EDirection NewDirection = CurrentDirection;
	if (FMath::Abs(Direction.X) > FMath::Abs(Direction.Y))
	{
		NewDirection = (Direction.X > 0.f) ? EDirection::Down : EDirection::Up;
	}
	else
	{
		NewDirection = (Direction.Y > 0.f) ? EDirection::Right : EDirection::Left;
	}

	// Если направление изменилось, то меняем спрайт
	//if (NewDirection != CurrentDirection)
	//{
		ChangeSprite(NewDirection);
	//}

	if (Jumping)
	{

		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("JUMP((("));
		this->Jump();

		// this->StopJumping();
	}

	TArray<AActor *> Towers_local;
	AActor *Tower_Closest = nullptr;
	float minDistance = 10000000000.f;
	UGameplayStatics::GetAllActorsOfClass(this, ATower::StaticClass(), Towers_local);
	for (auto Tower_local : Towers_local)
	{
		const float Distance = FVector::Distance(GetActorLocation(), Tower_local->GetActorLocation());
		if (Distance < minDistance)
		{
			minDistance = Distance;
			Tower_Closest = Tower_local;
		}
	}

	if (minDistance < 150.f)
	{
		Overlapped = true;
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Overlapped!"));

		OverlappedTower = CopyTemp(Cast<ATower>(Tower_Closest));
	}
	else
	{
		Overlapped = false;
		OverlappedTower = nullptr;
	}
	float CollectDistance = 100.f;
	TArray<AActor *> Coins;
	UGameplayStatics::GetAllActorsOfClass(this, ACoinBase::StaticClass(), Coins);
	for (auto coin : Coins)
	{
		const float Distance = FVector::Distance(GetActorLocation(), coin->GetActorLocation());
		if (Distance < CollectDistance)
		{
			ACoinBase *f = CopyTemp(Cast<ACoinBase>(coin));
			AddCoins(f->GetCost());
			coin->Destroy();
		}
	}
}
float AMyCharacter::GetCoins()
{
	return coinsAmount;
}
void AMyCharacter::AddCoins(float Coins)
{
	coinsAmount += Coins;
}
bool AMyCharacter::SpendCoins(float Coins)
{
	if (coinsAmount - Coins < 0)
	{
		return false;
	}
	coinsAmount -= Coins;
	return true;
}
void AMyCharacter::ChangeSprite(EDirection NewDirection)
{
	
	// Задаем новое направление
	CurrentDirection = NewDirection;
	if (NewDirection == EDirection::Left)
	{
		FlipbookComponent->SetRelativeRotation(FRotator(0, -90, 0));
	}
	else
	{
		FlipbookComponent->SetRelativeRotation(FRotator(0, 90, 0));
	}
	UPaperFlipbook *NewSprite = nullptr;
	// Получаем спрайт для нового направления из списка
	// GEngine->AddOnScreenDebugMessage(-1,1.f,FColor::White, FString::Printf());
	if (!bIsMovingV && !bIsMovingH){

		NewSprite = SpritesIdle.FindRef(NewDirection);
	}
	else
	{
		NewSprite = SpritesRun.FindRef(NewDirection);
	}

	// Если спрайт найден, то меняем его
	if (NewSprite != nullptr)
	{
		// Заменяем текущий спрайт на новый
		FlipbookComponent->SetFlipbook(NewSprite);
	}
	// Иначе выводим ошибку в лог
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Sprite not found for direction %d"), (int32)NewDirection);
	}
}

void AMyCharacter::CanStartNextWave(bool bCond)
{
	this->bCanStartWave = bCond;
}

void AMyCharacter::MoveVert(float AxisValue)
{
	if(AxisValue != 0){
		bIsMovingV = true;
	}else{
		bIsMovingV = false;
	}

	//GEngine->AddOnScreenDebugMessage(-1,15.f, FColor::White, FString::Printf(TEXT("VertAxis %i"),bIsMovingV));
	
	if (FMath::Abs(AxisValue) <= 0.1f)
	{
		bMoveVert = false;
		return;
	}
	bMoveVert = true;
	// this->AddMovementInput(GetActorForwardVector(), AxisValue);
	this->AddMovementInput(SpringArmComponent->GetForwardVector(), AxisValue);
	// TargetLocation = GetActorForwardVector() * GetWorld()->GetDeltaSeconds() * AxisValue * MoveSpeed + TargetLocation;
	Direction.X = AxisValue;
	Direction.Y = 0;
	if (AxisValue > 0)
	{
		if (!bMoveHoriz)
		{
			TowerMemberLocationShift.Y = 0;
		}

		TowerMemberLocationShift.X = TM_LOCATION_SHIFT;
	}
	else
	{

		if (!bMoveHoriz)
		{
			TowerMemberLocationShift.Y = 0;
		}

		TowerMemberLocationShift.X = -TM_LOCATION_SHIFT;
	}
}

void AMyCharacter::MoveHoriz(float AxisValue)
{
	if(AxisValue != 0){
		bIsMovingH = true;
	}else{
		
		bIsMovingH = false;
	}

	//GEngine->AddOnScreenDebugMessage(-1,15.f, FColor::White, FString::Printf(TEXT("HorizAxis %i"),bIsMovingH));
	

	if (FMath::Abs(AxisValue) <= 0.1f)
	{
		bMoveHoriz = false;
		return;
	}
	bMoveHoriz = true;
	Direction.Y = AxisValue;
	Direction.X = 0;
	AddMovementInput(SpringArmComponent->GetRightVector(), AxisValue);
	// TargetLocation = GetActorRightVector() * GetWorld()->GetDeltaSeconds() * AxisValue * MoveSpeed + TargetLocation;
	if (AxisValue > 0)
	{
		if (!bMoveVert)
		{
			TowerMemberLocationShift.X = 0;
		}

		TowerMemberLocationShift.Y = TM_LOCATION_SHIFT;
	}
	else
	{

		if (!bMoveVert)
		{
			TowerMemberLocationShift.X = 0;
		}
		TowerMemberLocationShift.Y = -TM_LOCATION_SHIFT;
	}
}

void AMyCharacter::Zoom(float AxisValue)
{
	if (AxisValue == 0.f)
	{
		return;
	}
	//

	const float Zoom = AxisValue * 500.f;
	TargetZoom = FMath ::Clamp(Zoom + TargetZoom, ARM_LENGTH_MIN, ARM_LENGTH_MAX);
}

void AMyCharacter::PlaceTower()
{
	if (TowerMember != nullptr)
	{

		if (bCanPlace)
		{
			// const int X = FMath::Clamp(TowerMemberLocation.X/TILE_SIDE_LEN, 0, Map->mapWidth -2) ;
			// const int Y = FMath::Clamp(TowerMemberLocation.Y/TILE_SIDE_LEN, 0, Map->mapHeight -2) ;
			int map_x = round(TowerMemberLocation.X / TILE_SIDE_LEN);
			int map_y = round(TowerMemberLocation.Y / TILE_SIDE_LEN);
			// // TowerMemberLocation.X = map_x * TILE_SIDE_LEN;
			// // TowerMemberLocation.Y = map_y * TILE_SIDE_LEN;
			// // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("%i"),
			// //	Map->Map_Cells[X][Y]));

			if (Map->Map_Cells[map_x][map_y] == 0)
			{
				TowerMember->SetActorLocation(TowerMemberLocation);
				TowerMember->SetPosXY(map_x, map_y);
				TowerMember->bIsSpawner = false;
				TowerMember->Placed = true;
				TowerMember = nullptr;
				Map->MapChange(map_x, map_y, this->PLACED);
				
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("X = %i, Y = %i"), map_x, map_y));
			}
			// else
			//  {
			//  	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("ЗАНЯТО!!!"));
			//  }
		}
	}
}

void AMyCharacter::SpawnTower(float DeltaTime)
{
	if (TowerMember != nullptr)
	{
		bCanPlace = false;
		int map_x = round((TowerMemberLocationShift.X + this->GetActorLocation().X) / TILE_SIDE_LEN);
		int map_y = round((TowerMemberLocationShift.Y + this->GetActorLocation().Y) / TILE_SIDE_LEN);
		if (Map->Map_Cells[map_x][map_y] == static_cast<int>(EMapEnum::Field))
		{
			TowerMemberLocation.X = map_x * TILE_SIDE_LEN;
			TowerMemberLocation.Y = map_y * TILE_SIDE_LEN;
		}
		// TowerMemberLocation.X = FMath::Clamp(TowerMemberLocation.X, 1, TILE_SIDE_LEN * (Map->mapWidth -2));
		// TowerMemberLocation.Y = FMath::Clamp(TowerMemberLocation.Y, 1, TILE_SIDE_LEN * (Map->mapHeight-2));

		const FVector Interpolated_Position = UKismetMathLibrary::VInterpTo(TowerMember->GetActorLocation(),
																			TowerMemberLocation,
																			DeltaTime, 15);

		TowerMember->SetActorLocation(Interpolated_Position);
		if (FVector::Distance(TowerMember->GetActorLocation(), TowerMemberLocation) < 20)
		{
			bCanPlace = true;
		}
		else
		{
			bCanPlace = false;
		}
	}
}

void AMyCharacter::ChangeTower()
{
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("ChangeTower()"));
	if (Overlapped)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Overlapped!"));
		if (OverlappedTower != nullptr)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Tower!!!"));
			if (OverlappedTower->bIsSpawner)
			{
				if (SpendCoins(OverlappedTower->GetCost()))
				{
					Tower = nullptr;
					Tower = OverlappedTower;
				}
				// SpendCoins(OverlappedTower->GetCost());

				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Statement Changed"));
			}
		}
	}

	if (this->Tower != nullptr && TowerMember == nullptr)
	{

		TowerMember = Tower->Place(); // f;
		TowerMember->bIsSpawner = false;
		this->Tower = nullptr;

		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("end of ChangeTower()"));
	}
}

void AMyCharacter::OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
								  UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
}

void AMyCharacter::OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp,
								int32 OtherBodyIndex)
{
}

void AMyCharacter::CheckJump()
{

	if (Jumping)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("already jumping"));
		Jumping = false;
	}
	else
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("JUMPED"));
		Jumping = true;
	}
}

void AMyCharacter::ApplyDamageOnCastle(float damageAmount)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%f"), CurrentCastleHealth));
	CurrentCastleHealth -= damageAmount;
}