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

	UserConstructionScript();
	
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

	for (UPrimitiveComponent* PrimitiveComponent : ColliderComponents)
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ASandstorm::OnBeginOverlap);
	
	if (AdvancementCurve)
	{
		AdvancementTimelineComponent->AddInterpFloat(AdvancementCurve, AdvancementUpdatedEvent, "AdvancementRatio");
		AdvancementTimelineComponent->SetTimelineFinishedFunc(AdvancementFinishedEvent);

		AdvancementTimelineComponent->Play();
	}
}

void ASandstorm::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (EmitterDistance == 0.f || !InitialSplineComponent)
		return;
	
	const int EmitterCount = UKismetMathLibrary::FTrunc(InitialSplineComponent->GetSplineLength() / EmitterDistance);

	FVector BoxExtent(EmitterDistance * 0.5f, 10.f, 1000.f);

	float SplineQuantity = EmitterDistance;
	FVector CurrentSplineLocation, NextSplineLocation;
	FVector CurrentSplineRight, NextSplineRight;

	CurrentSplineLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	CurrentSplineRight = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	
	for (int i = 0; i < EmitterCount; i++)
	{
		FVector SnappedEmitterPosition;
		
		if (GetSnappedWorldPosition(SnappedEmitterPosition, CurrentSplineLocation, SnapRange))
		{
			FVector EmitterDirection = CustomWorldDirection;
			
			if (DirectionType != ESandstormDirection::CUSTOM)
			{
				EmitterDirection = CurrentSplineRight;
				
				if (DirectionType == ESandstormDirection::CCW)
					EmitterDirection = -EmitterDirection;
			}
			
			FTransform EmitterRelativeTransform = UKismetMathLibrary::MakeTransform(SnappedEmitterPosition, UKismetMathLibrary::MakeRotFromX(EmitterDirection), FVector(EmitterScale));

			UActorComponent* AddedComponent = AddComponentByClass(UNiagaraComponent::StaticClass(), true, EmitterRelativeTransform, false);
			UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(AddedComponent);
			NiagaraComponent->SetAsset(NiagaraSystemAsset, false);
		}
		
		NextSplineLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(SplineQuantity, ESplineCoordinateSpace::World);
		NextSplineRight = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(SplineQuantity, ESplineCoordinateSpace::World);

		FVector WorldBoxLocation = (CurrentSplineLocation + NextSplineLocation) * 0.5f;

		FVector SnappedBoxLocation;
		if (bUseCollisions && GetSnappedWorldPosition(SnappedBoxLocation, WorldBoxLocation, SnapRange))
		{
			FVector BoxDirection = (CurrentSplineRight + NextSplineRight) * 0.5f;

			FTransform BoxTransform = UKismetMathLibrary::MakeTransform(SnappedBoxLocation, UKismetMathLibrary::MakeRotFromYZ(BoxDirection, FVector::UpVector), FVector::OneVector);
		
			UActorComponent* AddedComponent = AddComponentByClass(UBoxComponent::StaticClass(), true, BoxTransform, false);
			UBoxComponent* BoxComponent = Cast<UBoxComponent>(AddedComponent);
			BoxComponent->SetBoxExtent(BoxExtent, false);
		}
		
		SplineQuantity += EmitterDistance;

		CurrentSplineLocation = NextSplineLocation;
		CurrentSplineRight = NextSplineRight;
	}
}

void ASandstorm::OnAdvancementUpdate(float Ratio)
{
	const int EmitterCount = NiagaraComponents.Num();

	const float TargetDistance = TargetSplineComponent->GetSplineLength() / static_cast<float>(EmitterCount);

	float InitialSplineQuantity = EmitterDistance;
	float TargetSplineQuantity = TargetDistance;
	
	FVector InitCurrentLocation, InitNextLocation, TargetCurrentLocation, TargetNextLocation;
	FVector InitCurrentRight, InitNextRight, TargetCurrentRight, TargetNextRight;

	InitCurrentLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	InitCurrentRight = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	
	TargetCurrentLocation = TargetSplineComponent->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	TargetCurrentRight = TargetSplineComponent->GetRightVectorAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	
	for (int i = 0; i < EmitterCount; i++)
	{
		const FVector CurrentInterpolatedLocation = UKismetMathLibrary::VLerp(InitCurrentLocation, TargetCurrentLocation, Ratio);
		const FVector CurrentInterpolatedRight = UKismetMathLibrary::VLerp(InitCurrentRight, TargetCurrentRight, Ratio);
		
		FVector SnappedEmitterPosition;
		
		if (GetSnappedWorldPosition(SnappedEmitterPosition, CurrentInterpolatedLocation, SnapRange))
		{
			FVector EmitterDirection = CustomWorldDirection;
			
			if (DirectionType != ESandstormDirection::CUSTOM)
			{
				EmitterDirection = UKismetMathLibrary::ProjectVectorOnToPlane(CurrentInterpolatedRight, FVector::UpVector);

				if (DirectionType == ESandstormDirection::CW)
					EmitterDirection = -EmitterDirection;
			}
			
			FTransform EmitterWorldTransform = UKismetMathLibrary::MakeTransform(SnappedEmitterPosition, UKismetMathLibrary::MakeRotFromX(EmitterDirection), FVector(EmitterScale));

			NiagaraComponents[i]->SetWorldTransform(EmitterWorldTransform);
		}
		
		InitNextLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(InitialSplineQuantity, ESplineCoordinateSpace::World);
		InitNextRight = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(InitialSplineQuantity, ESplineCoordinateSpace::World);

		TargetNextLocation = TargetSplineComponent->GetLocationAtDistanceAlongSpline(TargetSplineQuantity, ESplineCoordinateSpace::World);
		TargetNextRight = TargetSplineComponent->GetRightVectorAtDistanceAlongSpline(TargetSplineQuantity, ESplineCoordinateSpace::World);

		const FVector NextInterpolatedLocation = UKismetMathLibrary::VLerp(InitNextLocation, TargetNextLocation, Ratio);
		const FVector NextInterpolatedRight = UKismetMathLibrary::VLerp(InitNextRight, TargetNextRight, Ratio);

		FVector WorldBoxLocation = (CurrentInterpolatedLocation + NextInterpolatedLocation) * 0.5f;

		FVector SnappedBoxLocation;
		if (bUseCollisions && GetSnappedWorldPosition(SnappedBoxLocation, WorldBoxLocation, SnapRange))
		{
			FVector BoxDirection = (CurrentInterpolatedRight + NextInterpolatedRight) * 0.5f;
	
			FTransform BoxTransform = UKismetMathLibrary::MakeTransform(SnappedBoxLocation, UKismetMathLibrary::MakeRotFromYZ(BoxDirection, FVector::UpVector), FVector::OneVector);
		
			ColliderComponents[i]->SetWorldTransform(BoxTransform);
		}
		
		InitialSplineQuantity += EmitterDistance;
		TargetSplineQuantity += TargetDistance;
		
		InitCurrentLocation = InitNextLocation;
		TargetCurrentLocation = TargetNextLocation;
		
		InitCurrentRight = InitNextRight;
		TargetCurrentRight = TargetNextRight;
	}
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
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%s"), *OtherActor->GetName()));
}
