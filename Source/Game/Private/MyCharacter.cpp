// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "PaperSprite.h"
#include "Components/CapsuleComponent.h"
#include "Components/PostProcessComponent.h"

#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#define DefaultRotationPitch -45
#define TM_LOCATION_SHIFT 220
#define TILE_SIDE_LEN 200
#define ARM_LENGTH_MIN 2000.f
#define ARM_LENGTH_MAX 15000.f
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
    bReadyForWave = true;
	this->GetMesh()->SetupAttachment(RootComponent);
///
///	SpringArm setup
/// 
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->TargetArmLength = ARM_LENGTH_MIN;
	SpringArmComponent->bDoCollisionTest = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->FieldOfView = FOV;
///
///	Collision setup
/// 
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComponent"));
	Collision->SetupAttachment(RootComponent);
	Collision->SetBoxExtent(FVector(32.f,32.f,32.f));
	Collision->SetWorldScale3D(FVector(2.f,2.f,2.f));
	Collision->SetRelativeLocation(FVector(0.f,0.f,50.f));
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AMyCharacter::OnOverlapEnd);
	Collision->SetCollisionProfileName(TEXT("Trigger"));
		
	this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	
///
/// Sprite setup
///

	UPaperSprite* UpSprite = LoadObject<UPaperSprite>(nullptr, TEXT("/Script/Paper2D.PaperSprite'/Game/_Main/Sprites/Enemies/sprite_sheet_Sprite_77.sprite_sheet_Sprite_77'"));
	UPaperSprite* DownSprite = LoadObject<UPaperSprite>(nullptr,TEXT("/Script/Paper2D.PaperSprite'/Game/_Main/Sprites/Enemies/sprite_sheet_Sprite_78.sprite_sheet_Sprite_78'"));
	UPaperSprite* LeftSprite = LoadObject<UPaperSprite>(nullptr,TEXT("/Script/Paper2D.PaperSprite'/Game/_Main/Sprites/Enemies/sprite_sheet_Sprite_76.sprite_sheet_Sprite_76'"));
	UPaperSprite* RightSprite = LoadObject<UPaperSprite>(nullptr,TEXT("/Script/Paper2D.PaperSprite'/Game/_Main/Sprites/Enemies/sprite_sheet_Sprite_75.sprite_sheet_Sprite_75'"));
	
	
	Sprites.Add(EDirection::Up, UpSprite);
	Sprites.Add(EDirection::Down, DownSprite);
	Sprites.Add(EDirection::Left, LeftSprite);
	Sprites.Add(EDirection::Right, RightSprite);

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpriteComponent->SetupAttachment(RootComponent);
	SpriteComponent->SetGenerateOverlapEvents(false);


///
/// Post Process Component
///

	/*PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));

	PostProcessComponent->Settings.DepthOfFieldSensorWidth = 144.f;
	PostProcessComponent->Settings.DepthOfFieldFocalDistance = SpringArmComponent->TargetArmLength;
	PostProcessComponent->Settings.DepthOfFieldFstop = 0.8f;
	*/
	
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	
	Super::BeginPlay();
	
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
	Map->SetTileSize(TILE_SIDE_LEN);
	Map->SpawnMap();
	Map->ProcessWaypoints();
	 
	Jumping = false;

	// Устанавливаем начальный спрайт
	CurrentSprite = Sprites[EDirection::Right];
	SpriteComponent->SetRelativeRotation(FRotator(90.0f, 90.f, 0.0f));
	SpriteComponent->SetSprite(CurrentSprite);
	SpriteComponent->SetWorldScale3D(FVector(10.0f, 10.0f, 10.0f));


	
	this->SetActorLocation(FVector(Map->StartCell.Y_coord*TILE_SIDE_LEN,Map->StartCell.X_coord*TILE_SIDE_LEN,0));


	
}
// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveVert"), this, &AMyCharacter::MoveVert);
	PlayerInputComponent->BindAxis(TEXT("MoveHoriz"), this, &AMyCharacter::MoveHoriz);
	PlayerInputComponent->BindAxis(TEXT("CamZoom"), this, &AMyCharacter::Zoom);

	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::CheckJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::CheckJump);
	 
	PlayerInputComponent->BindAction(TEXT("Action|Take"), IE_Pressed, this, &AMyCharacter::ChangeTower);
	PlayerInputComponent->BindAction(TEXT("Action|Place"), IE_Pressed, this, &AMyCharacter::PlaceTower);
	
}
// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, ZoomSpeed);
	this->SpringArmComponent->TargetArmLength = InterpolatedZoom;
	//PostProcessComponent->Settings.DepthOfFieldFocalDistance = SpringArmComponent->TargetArmLength;
	
	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, RotateSpeed);
	SpringArmComponent->SetRelativeRotation(InterpolatedRotation);

	this->SpawnTower(DeltaTime);

	
	// Определяем новое направление спрайта в зависимости от направления движения
	
	
	EDirection NewDirection;
	if (FMath::Abs(Direction.X) > FMath::Abs(Direction.Y))
	{
		NewDirection = (Direction.X > 0.f) ? EDirection::Right : EDirection::Left;
	}
	else
	{
		NewDirection = (Direction.Y > 0.f) ? EDirection::Down : EDirection::Up;
	}

	// Если направление изменилось, то меняем спрайт
	if (NewDirection != CurrentDirection)
	{
		ChangeSprite(NewDirection);
	}

	
	if(Jumping)
	{
		
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("JUMP((("));
        this->Jump();
		
		//this->StopJumping();
	}
}
void AMyCharacter::ChangeSprite(EDirection NewDirection)
{
	// Задаем новое направление
	CurrentDirection = NewDirection;

	// Получаем спрайт для нового направления из списка
	UPaperSprite* NewSprite = Sprites.FindRef(NewDirection);

	// Если спрайт найден, то меняем его
	if (NewSprite != nullptr)
	{
		// Заменяем текущий спрайт на новый
		SpriteComponent->SetSprite(NewSprite);
	}
	// Иначе выводим ошибку в лог
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Sprite not found for direction %d"), (int32)NewDirection);
	}
}


void AMyCharacter::MoveVert(float AxisValue)
{
	
	if(AxisValue == 0.f){
		bMoveVert = false;
		return;
	}
	bMoveVert = true;
	this->AddMovementInput(GetActorForwardVector(), AxisValue);
 
	//TargetLocation = GetActorForwardVector() * GetWorld()->GetDeltaSeconds() * AxisValue * MoveSpeed + TargetLocation;
	Direction.X = AxisValue;
	Direction.Y = 0;
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

void AMyCharacter::MoveHoriz(float AxisValue)
{
	
	if(AxisValue == 0.f){
		bMoveHoriz = false;
		return;
	}
	bMoveHoriz = true;
	Direction.Y = AxisValue;
	Direction.X = 0;
	AddMovementInput(GetActorRightVector() ,AxisValue);
	//TargetLocation = GetActorRightVector() * GetWorld()->GetDeltaSeconds() * AxisValue * MoveSpeed + TargetLocation;
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

void AMyCharacter::Zoom(float AxisValue)
{
	if(AxisValue == 0.f)
	{
		return;
	}
	//

	const float Zoom = AxisValue * 500.f;
	TargetZoom = FMath :: Clamp(Zoom + TargetZoom, ARM_LENGTH_MIN, ARM_LENGTH_MAX);
	
}

void AMyCharacter::PlaceTower()
{
	if(TowerMember!=nullptr)
	{
		
		if(bCanPlace)
        {
			const int X = FMath::Clamp(TowerMemberLocation.X/TILE_SIDE_LEN, 0, Map->mapWidth -2) ;
			const int Y = FMath::Clamp(TowerMemberLocation.Y/TILE_SIDE_LEN, 0, Map->mapHeight -2) ;
			TowerMemberLocation.X = round((TowerMemberLocationShift.X + this->GetActorLocation().X) / TILE_SIDE_LEN) * TILE_SIDE_LEN;
			TowerMemberLocation.Y = round((TowerMemberLocationShift.Y + this->GetActorLocation().Y) / TILE_SIDE_LEN) * TILE_SIDE_LEN;
            TowerMemberLocation.X = FMath::Clamp(TowerMemberLocation.X, 0, TILE_SIDE_LEN * (Map->mapWidth-1));
            TowerMemberLocation.Y = FMath::Clamp(TowerMemberLocation.Y, 0, TILE_SIDE_LEN * (Map->mapHeight-1));
 
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("%i"),
			//	Map->Map_Cells[X][Y]));

			if(Map->Map_Cells[X][Y]==0)
            {
            	TowerMember->SetActorLocation(TowerMemberLocation);
				TowerMember->bIsSpawner=false;
				TowerMember->Placed = true;
            	TowerMember = nullptr;
				Map->MapChange(X,Y, this->PLACED);
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("X = %i, Y = %i"),
				X,Y));
            }else
            {
            	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("ЗАНЯТО!!!"));
            }
			
			
        }
		
		
		
	}
}

void AMyCharacter::SpawnTower(float DeltaTime)
{
	if(TowerMember != nullptr)
	{
		TowerMemberLocation.X = round((TowerMemberLocationShift.X + this->GetActorLocation().X) / TILE_SIDE_LEN) * TILE_SIDE_LEN;
		TowerMemberLocation.Y = round((TowerMemberLocationShift.Y + this->GetActorLocation().Y) / TILE_SIDE_LEN) * TILE_SIDE_LEN;
		TowerMemberLocation.X = FMath::Clamp(TowerMemberLocation.X, 0, TILE_SIDE_LEN * (Map->mapWidth -2));
		TowerMemberLocation.Y = FMath::Clamp(TowerMemberLocation.Y, 0, TILE_SIDE_LEN * (Map->mapHeight-2));

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

void AMyCharacter::ChangeTower()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("ChangeTower()"));
	if(Overlapped)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Overlapped!"));
		if (OverlappedTower!=nullptr)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Tower!!!"));
			if(OverlappedTower->bIsSpawner)
			{
				
					Tower = nullptr;
                    Tower = OverlappedTower;
				
                			
                //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Statement Changed"));
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

void AMyCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor!=this)
	{
		Overlapped = true;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Overlapped!"));
		
        OverlappedTower = CopyTemp(Cast<ATower>(OtherActor));
	}
	
	
	                                          	
}

void AMyCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if(OtherActor!=this)
	{
		Overlapped = false;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Black, TEXT("closed!"));
	}	
}

void AMyCharacter::CheckJump()
{
	
	if(Jumping)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("already jumping"));
		Jumping = false;
	}else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("JUMPED"));
		Jumping = true;
	}
}






