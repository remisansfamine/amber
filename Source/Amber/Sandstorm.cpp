// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandstorm.h"
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASandstorm::ASandstorm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	InitialSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("InitialSpline"));
	InitialSplineComponent->SetupAttachment(RootComponent);
	InitialSplineComponent->bInputSplinePointsToConstructionScript = true; 

	TargetSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("TargetSpline"));
	TargetSplineComponent->SetupAttachment(RootComponent);
	TargetSplineComponent->bInputSplinePointsToConstructionScript = true;
	
	AdvancementTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("AdvancementTimeline"));
	
	AdvancementUpdatedEvent.BindDynamic(this, &ASandstorm::OnAdvancementUpdate);
    AdvancementFinishedEvent.BindDynamic(this, &ASandstorm::OnAdvancementFinished);
}

// Called when the game starts or when spawned
void ASandstorm::BeginPlay()
{
	Super::BeginPlay();

	GetComponents<UNiagaraComponent>(NiagaraComponents);
	GetComponents<UBoxComponent>(ColliderComponents);
	
	if (AdvancementCurve)
	{
		AdvancementTimelineComponent->AddInterpFloat(AdvancementCurve, AdvancementUpdatedEvent, "AdvancementRatio");
		AdvancementTimelineComponent->SetTimelineFinishedFunc(AdvancementFinishedEvent);

		AdvancementTimelineComponent->Play();
	}
}

void ASandstorm::AddColliderComponents(float SplineLength)
{
	FVector CurrentSplineLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	FVector CurrentSplineRight = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);

	const float ColliderDistance = ColliderScale.X * 2.f;
	const int ColliderCount = UKismetMathLibrary::FTrunc(SplineLength / ColliderDistance);

	float ColliderSplineQuantity = 0.f;
	for (int i = 0; i < ColliderCount; i++)
	{
		FVector NextSplineLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(ColliderSplineQuantity, ESplineCoordinateSpace::World);
		FVector NextSplineRight = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(ColliderSplineQuantity, ESplineCoordinateSpace::World);

		FVector WorldBoxLocation = (CurrentSplineLocation + NextSplineLocation) * 0.5f;

		FVector SnappedBoxLocation;
		if (bUseCollisions && GetSnappedWorldPosition(SnappedBoxLocation, WorldBoxLocation, SnapRange))
		{
			FVector BoxDirection = (CurrentSplineRight + NextSplineRight) * 0.5f;

			FTransform BoxTransform = UKismetMathLibrary::MakeTransform(SnappedBoxLocation, UKismetMathLibrary::MakeRotFromYZ(BoxDirection, FVector::UpVector), FVector::OneVector);
		
			UActorComponent* AddedComponent = AddComponentByClass(UBoxComponent::StaticClass(), true, BoxTransform, false);
			UBoxComponent* BoxComponent = Cast<UBoxComponent>(AddedComponent);
			BoxComponent->SetBoxExtent(ColliderScale, false);
			BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASandstorm::OnBeginOverlap);
		}
		
		ColliderSplineQuantity += ColliderDistance;

		CurrentSplineLocation = NextSplineLocation;
		CurrentSplineRight = NextSplineRight;
	}
}

void ASandstorm::AddEmitterComponents(float SplineLength)
{
	const int EmitterCount = UKismetMathLibrary::FTrunc(SplineLength / EmitterDistance);

	float EmitterSplineQuantity = 0.f;
	for (int i = 0; i < EmitterCount; i++)
	{
		FVector CurrentSplineLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(EmitterSplineQuantity, ESplineCoordinateSpace::World);

		FVector SnappedEmitterPosition;
		if (GetSnappedWorldPosition(SnappedEmitterPosition, CurrentSplineLocation, SnapRange))
		{
			FVector EmitterDirection = CustomWorldDirection;
			
			if (DirectionType != ESandstormDirection::CUSTOM)
			{
				EmitterDirection = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(EmitterSplineQuantity, ESplineCoordinateSpace::World);
				
				if (DirectionType == ESandstormDirection::CCW)
					EmitterDirection = -EmitterDirection;
			}
			
			FTransform EmitterRelativeTransform = UKismetMathLibrary::MakeTransform(SnappedEmitterPosition, UKismetMathLibrary::MakeRotFromX(EmitterDirection), FVector(EmitterScale));

			UActorComponent* AddedComponent = AddComponentByClass(UNiagaraComponent::StaticClass(), true, EmitterRelativeTransform, false);
			UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(AddedComponent);
			NiagaraComponent->SetAsset(NiagaraSystemAsset, false);
		}

		EmitterSplineQuantity += EmitterDistance;
	}
}

void ASandstorm::AddComponents()
{
	const float SplineLength = InitialSplineComponent->GetSplineLength();
	
	AddEmitterComponents(SplineLength);
	AddColliderComponents(SplineLength);
}

void ASandstorm::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (EmitterDistance != 0.f && InitialSplineComponent)
		AddComponents();
}

void ASandstorm::MoveEmitters(float Ratio, float TargetSplineLength)
{
	const int EmitterCount = NiagaraComponents.Num();
	const float EmitterTargetDistance = TargetSplineLength / static_cast<float>(EmitterCount);

	float EmitterInitialSplineQuantity = 0.f, EmitterTargetSplineQuantity = 0.f;
	for (int i = 0; i < EmitterCount; i++)
	{
		const FVector InitCurrentLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(EmitterInitialSplineQuantity, ESplineCoordinateSpace::World);
		const FVector TargetCurrentLocation = TargetSplineComponent->GetLocationAtDistanceAlongSpline(EmitterTargetSplineQuantity, ESplineCoordinateSpace::World);
		
		const FVector CurrentInterpolatedLocation = UKismetMathLibrary::VLerp(InitCurrentLocation, TargetCurrentLocation, Ratio);
		
		FVector SnappedEmitterPosition;
		if (GetSnappedWorldPosition(SnappedEmitterPosition, CurrentInterpolatedLocation, SnapRange))
		{
			FVector EmitterDirection = CustomWorldDirection;
			
			if (DirectionType != ESandstormDirection::CUSTOM)
			{
				const FVector InitCurrentRight = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(EmitterInitialSplineQuantity, ESplineCoordinateSpace::World);
				const FVector TargetCurrentRight = TargetSplineComponent->GetRightVectorAtDistanceAlongSpline(EmitterTargetSplineQuantity, ESplineCoordinateSpace::World);

				const FVector CurrentInterpolatedRight = UKismetMathLibrary::VLerp(InitCurrentRight, TargetCurrentRight, Ratio);

				EmitterDirection = UKismetMathLibrary::ProjectVectorOnToPlane(CurrentInterpolatedRight, FVector::UpVector);

				if (DirectionType == ESandstormDirection::CW)
					EmitterDirection = -EmitterDirection;
			}
			
			FTransform EmitterWorldTransform = UKismetMathLibrary::MakeTransform(SnappedEmitterPosition, UKismetMathLibrary::MakeRotFromX(EmitterDirection), FVector(EmitterScale));

			NiagaraComponents[i]->SetWorldTransform(EmitterWorldTransform);
		}

		EmitterInitialSplineQuantity += EmitterDistance;
		EmitterTargetSplineQuantity += EmitterTargetDistance;
	}
}
void ASandstorm::MoveColliders(float Ratio, float TargetSplineLength)
{
	FVector InitCurrentLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	FVector InitCurrentRight = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	
	FVector TargetCurrentLocation = TargetSplineComponent->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	FVector TargetCurrentRight = TargetSplineComponent->GetRightVectorAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	
	FVector InitNextLocation, TargetNextLocation;
	FVector InitNextRight, TargetNextRight;
	
	const int ColliderCount = ColliderComponents.Num();
	const float ColliderInitDistance = ColliderScale.X * 2.f;
	const float ColliderTargetDistance = TargetSplineLength / static_cast<float>(ColliderCount);

	const float InterpolatedDistance = UKismetMathLibrary::Lerp(ColliderInitDistance, ColliderTargetDistance, Ratio);
	const FVector NewColliderScale = FVector(InterpolatedDistance * 0.5f, ColliderScale.Y, ColliderScale.Z);
	
	float ColliderInitialSplineQuantity = 0.f, ColliderTargetSplineQuantity = 0.f;
	for (int i = 0; i < ColliderCount; i++)
	{
		const FVector CurrentInterpolatedLocation = UKismetMathLibrary::VLerp(InitCurrentLocation, TargetCurrentLocation, Ratio);
		const FVector CurrentInterpolatedRight = UKismetMathLibrary::VLerp(InitCurrentRight, TargetCurrentRight, Ratio);
		
		InitNextLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(ColliderInitialSplineQuantity, ESplineCoordinateSpace::World);
		InitNextRight = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(ColliderInitialSplineQuantity, ESplineCoordinateSpace::World);

		TargetNextLocation = TargetSplineComponent->GetLocationAtDistanceAlongSpline(ColliderTargetSplineQuantity, ESplineCoordinateSpace::World);
		TargetNextRight = TargetSplineComponent->GetRightVectorAtDistanceAlongSpline(ColliderTargetSplineQuantity, ESplineCoordinateSpace::World);

		const FVector NextInterpolatedLocation = UKismetMathLibrary::VLerp(InitNextLocation, TargetNextLocation, Ratio);
		const FVector NextInterpolatedRight = UKismetMathLibrary::VLerp(InitNextRight, TargetNextRight, Ratio);

		FVector WorldBoxLocation = (CurrentInterpolatedLocation + NextInterpolatedLocation) * 0.5f;

		FVector SnappedBoxLocation;
		if (bUseCollisions && GetSnappedWorldPosition(SnappedBoxLocation, WorldBoxLocation, SnapRange))
		{
			FVector BoxDirection = (CurrentInterpolatedRight + NextInterpolatedRight) * 0.5f;

			FTransform BoxTransform = UKismetMathLibrary::MakeTransform(SnappedBoxLocation, UKismetMathLibrary::MakeRotFromYZ(BoxDirection, FVector::UpVector), FVector::OneVector);
	
			UBoxComponent* BoxComponent = ColliderComponents[i];
			BoxComponent->SetWorldTransform(BoxTransform);
			BoxComponent->SetBoxExtent(NewColliderScale, false);
		}
		
		ColliderInitialSplineQuantity += ColliderInitDistance;
		ColliderTargetSplineQuantity += ColliderTargetDistance;
		
		InitCurrentLocation = InitNextLocation;
		TargetCurrentLocation = TargetNextLocation;
		
		InitCurrentRight = InitNextRight;
		TargetCurrentRight = TargetNextRight;
	}
}

void ASandstorm::OnAdvancementUpdate(float Ratio)
{
	const float TargetSplineLength = TargetSplineComponent->GetSplineLength();
	
	MoveEmitters(Ratio, TargetSplineLength);
	MoveColliders(Ratio, TargetSplineLength);
}

bool ASandstorm::GetSnappedWorldPosition(FVector& SnappedWorldPosition, const FVector& WorldPosition, const float Range) const
{
	const FVector Start = FVector(WorldPosition.X, WorldPosition.Y, WorldPosition.Z + Range);
	const FVector End = FVector(WorldPosition.X, WorldPosition.Y, WorldPosition.Z - Range);

	FHitResult OutHit;
	
	if (!GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility))
		return false;

	SnappedWorldPosition = OutHit.ImpactPoint;

	return true;
}

bool ASandstorm::GetSnappedRelativePosition(FVector& SnappedRelativePosition, const FVector& WorldPosition, const float Range) const
{
	if (!GetSnappedWorldPosition(SnappedRelativePosition, WorldPosition, Range))
		return false;
	
	SnappedRelativePosition = GetActorTransform().InverseTransformPosition(SnappedRelativePosition);
	
	return true; 
}

void ASandstorm::OnAdvancementFinished()
{
	Destroy();
}

void ASandstorm::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this)
		return;

	if (OnSandstormEnter.IsBound())
		OnSandstormEnter.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
