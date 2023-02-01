// Fill out your copyright notice in the Description page of Project Settings.


#include "Quicksands.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AQuicksands::AQuicksands()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMeshComponent->SetupAttachment(RootComponent);
	CubeMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(CubeMeshComponent);
	CubeMeshComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));


	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AQuicksands::OnBoxOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AQuicksands::OnBoxOverlapEnd);
}

// Called when the game starts or when spawned
void AQuicksands::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AQuicksands::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Prey)
		ComputePreyStats();
}

void AQuicksands::ComputePreyStats() 
{
	UCharacterMovementComponent* PreyMovement = Prey->GetCharacterMovement();

	const float AlgebraicDistance = FMath::Abs(Prey->GetActorLocation().Z - ImpactPointHeight);
	
	const float SubmersionRatio = FMath::Exp(-ExpMultiplier * AlgebraicDistance);
	
	PreyMovement->GravityScale = FMath::Lerp(MinGravityScale, 1.f, SubmersionRatio);
	PreyMovement->MaxWalkSpeed = FMath::Lerp(MinMaxSpeedScale, InitialWalkSpeed, SubmersionRatio);
}

void AQuicksands::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Prey)
		return;
	
	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);

	if (!OtherCharacter)
		return;

	Prey = OtherCharacter;
	InitialWalkSpeed = Prey->GetCharacterMovement()->MaxWalkSpeed;
	ImpactPointHeight = Prey->GetActorLocation().Z;
}

void AQuicksands::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Prey != OtherActor)
		return;
	
	Prey->GetCharacterMovement()->MaxWalkSpeed = InitialWalkSpeed;
	Prey->GetCharacterMovement()->GravityScale = 1.f;
	
	Prey = nullptr;
}
