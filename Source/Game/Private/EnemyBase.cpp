// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"

#include "EnemyAIController.h"
#include "PaperSprite.h"
#include "PaperFlipbook.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#define SPEED 200.f
// Sets default values
AEnemyBase::AEnemyBase()
{
	/*
	 *TODO: create sprites and uncomment this section*/
	UPaperFlipbook *UpSprite = LoadObject<UPaperFlipbook>(
		nullptr, TEXT(
					 "/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/Enemies/Left.Left'"));
	UPaperFlipbook *DownSprite = LoadObject<UPaperFlipbook>(
		nullptr, TEXT(
					 "/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/Enemies/Right.Right'"));
	UPaperFlipbook *LeftSprite = LoadObject<UPaperFlipbook>(
		nullptr, TEXT(
					 "/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/Enemies/Front.Front'"));
	UPaperFlipbook *RightSprite = LoadObject<UPaperFlipbook>(
		nullptr, TEXT(
					 "/Script/Paper2D.PaperFlipbook'/Game/_Main/Sprites/Enemies/Back.Back'"));

	Sprites.Add(EDirection::Up, UpSprite);
	Sprites.Add(EDirection::Down, DownSprite);
	Sprites.Add(EDirection::Left, LeftSprite);
	Sprites.Add(EDirection::Right, RightSprite);

	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Создаем компонент спрайта
	SpriteComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpriteComponent"));
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpriteComponent->SetupAttachment(RootComponent);
	SpriteComponent->SetGenerateOverlapEvents(false);
	SpriteComponent->SetRelativeLocation(FVector(0,0,-190.f));
	SpriteComponent->SetCastShadow(true);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSphereRadius(100.f);						  // задаем радиус сферы
	SphereCollision->SetCollisionProfileName(TEXT("EnemyCollision")); // задаем профиль коллизии для сферы
	SphereCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// take damage
	OnTakeAnyDamage.AddDynamic(this, &AEnemyBase::OnAnyDamage);

	EnemyAIController = Cast<AEnemyAIController>(GetController());

	const FVector MapLocation(0.0f, 0.0f, 0.0f);
	const FRotator MapRotation(0.0f, 0.0f, 0.0f);
	const FActorSpawnParameters SpawnInfo;
	Map = GetWorld()->SpawnActor<AMyMap>(MapLocation, MapRotation, SpawnInfo);
	Map->SetTileSize(TILE_SIDE_LEN);
	Map->ProcessWaypoints();

	this->SetActorLocation(FVector(Map->StartCell.Y_coord * TILE_SIDE_LEN, Map->StartCell.X_coord * TILE_SIDE_LEN,
								   TILE_SIDE_LEN));

	// Устанавливаем начальный спрайт
	CurrentSprite = Sprites[EDirection::Right];
	SpriteComponent->SetRelativeRotation(FRotator(0.0f, 90.f, 0.0f));
	SpriteComponent->SetFlipbook(CurrentSprite);
	SpriteComponent->SetWorldScale3D(FVector(5.0f, 5.0f, 5.0f));

	CurrentCheckPoint = 0;
	canMove = true;

	SetMaxHealth(100.f);
	SetHealth(GetMaxHealth());
}

void AEnemyBase::ChangeSprite(EDirection NewDirection)
{
	// Задаем новое направление
	CurrentDirection = NewDirection;

	// Получаем спрайт для нового направления из списка
	UPaperFlipbook *NewSprite = Sprites.FindRef(NewDirection);

	// Если спрайт найден, то меняем его
	if (NewSprite != nullptr)
	{
		// Заменяем текущий спрайт на новый
		SpriteComponent->SetFlipbook(NewSprite);
	}
	// Иначе выводим ошибку в лог
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Sprite not found for direction %d"), (int32)NewDirection);
	}
}

void AEnemyBase::OnAnyDamage(AActor *DamagedActor, float Damage, const UDamageType *DamageType,
							 AController *InstigatedBy, AActor *DamageCauser)
{
	if (DamagedActor == this)
	{
		this->AddDamage(Damage);
		// this->SetHealth(GetHealth()-Damage);
		UE_LOG(LogTemp, Warning, TEXT("Damage received: %f"), Damage);
	}
}

void AEnemyBase::MoveBetweenPoints(float DeltaTime)
{
	if (canMove && CurrentCheckPoint < Map->Nodes.Num())
	{
		const FVector NextLocation = FVector(Map->Nodes[CurrentCheckPoint].Y_coord * TILE_SIDE_LEN,
											 Map->Nodes[CurrentCheckPoint].X_coord * TILE_SIDE_LEN, TILE_SIDE_LEN);

		const float DistanceToTarget = FVector::Distance(NextLocation, GetActorLocation());

		if (DistanceToTarget > 10.f)
		{
			const FVector Direction = (NextLocation - GetActorLocation()).GetSafeNormal();
			SetActorLocation(GetActorLocation() + Direction * MoveSpeed * DeltaTime);
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("point number %i"), CurrentCheckPoint));
			//  Определяем новое направление спрайта в зависимости от направления движения
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
		}
		else
		{
			CurrentCheckPoint++;
		}
	}
	else
	{
		canMove = false;
	}
	if (!this->isDead)
		if (CurrentCheckPoint >= Map->Nodes.Num())
		{
			isDamaging = true;
			// GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &AEnemyBase::DamageCastle, DamageDelay, true, 0.2f);
		}
}

void AEnemyBase::DamageCastle()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, FString::Printf(TEXT("DAMAGING")));
	GetPlayer()->ApplyDamageOnCastle(DamageAmount);
}
// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/// check if alive
	/*if(GetHealth()<0)
	{
		Destroy();
		return;
	}*/

	if(isDamaging){
		timer_ += DeltaTime;
		if(timer_ >= DamageDelay){
			timer_ = 0;
			DamageCastle();
		}
	}

	MoveBetweenPoints(DeltaTime);
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyBase::MoveToWaypoint()
{
	Moving = true;

	if (canMove && EnemyAIController && CurrentCheckPoint < Map->Nodes.Num())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Emerald, FString::Printf(TEXT("Moving? %i"), Moving));
		const FVector NextLocation = FVector(Map->Nodes[CurrentCheckPoint].Y_coord * TILE_SIDE_LEN,
											 Map->Nodes[CurrentCheckPoint].X_coord * TILE_SIDE_LEN, TILE_SIDE_LEN);
		TargetLocation = NextLocation;
		EnemyAIController->MoveToLocation(NextLocation, 5.f, false);

		CurrentCheckPoint++;
	}
	else
	{
		canMove = false;
	}
}

AMyCharacter *AEnemyBase::GetPlayer() const
{
	if (GetWorld())
	{
		AMyCharacter *Mallet = Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (!Mallet)
		{
			return nullptr;
		}
		Mallet->Map->ProcessWaypoints();
		return Mallet;
	}
	return nullptr;
}

void AEnemyBase::Die(float TimeBeforeDie)
{
	// death logic
	canMove = false;
	isDead = true;
	isDamaging = false;
	Map->Destroy();

	SpawnCoins();
	// death timer

	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("dieing")));
	SetLifeSpan(TimeBeforeDie);
}

void AEnemyBase::SpawnCoins()
{

	if(CoinsSpawned){
		return;
	}
	CoinsSpawned = true;
	int coinsize = 10;
	int amount = this->GetReward() / coinsize;

	FRandomStream RandomStream(GetUniqueID());

	float Length = FMath::RandRange(20.f, 50.f);

	for (int i = 0; i < amount; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		CoinClass = ACoinBase::StaticClass();
		ACoinBase *Coin = GetWorld()->SpawnActor<ACoinBase>(CoinClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		float Speed = 250.f;
		Coin->SetSpeed(Speed);
		if (Coin)
		{

			// Set the projectile's initial trajectory.
			FVector LaunchDirection; // = (FMath::Rand()).GetSafeNormal();

			float RandomX = FMath::RandRange(-1.0f, 1.0f);
			float RandomY = FMath::RandRange(-1.0f, 1.0f);
			float RandomZ = FMath::RandRange(0.5f, 1.0f);
			LaunchDirection = FVector(RandomX, RandomY, RandomZ);
			LaunchDirection *= Length;
			Coin->SetVelocity(LaunchDirection);
		}
	}
	
}

void AEnemyBase::SetReward(float Amount)
{
	Reward = Amount;
}
auto AEnemyBase::Place() -> AEnemyBase *
{
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.Template = this;
		return GetWorld()->SpawnActor<AEnemyBase>(this->GetClass(), SpawnParameters);
	}

	return nullptr;
}

void AEnemyBase::AddDamage(float Damage)
{
	this->SetHealth(this->GetHealth() - Damage);
	if (Health <= 0 && !isDead)
	{
		this->Die(1);
	}
}

void AEnemyBase::SetHealth(float Amount)
{
	Health = Amount;
	FMath::Clamp(Health, 0, GetMaxHealth());
}

float AEnemyBase::GetHealth()
{
	return Health;
}

float AEnemyBase::GetMaxHealth()
{
	return MaxHealth;
}

void AEnemyBase::SetMaxHealth(float Amount)
{
	this->MaxHealth = Amount;
}

float AEnemyBase::GetReward()
{
	return Reward;
}
