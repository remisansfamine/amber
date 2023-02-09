// Fill out your copyright notice in the Description page of Project Settings.


#include "Sandstorm.h"
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
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

	for (int i = 0; i < EmitterCount; i++)
	{
		const float SplineQuantity = EmitterDistance * i;

		FVector WorldLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(SplineQuantity, ESplineCoordinateSpace::World);

		FVector SnappedWorldPosition;
		
		if (!GetSnappedWorldPosition(SnappedWorldPosition, WorldLocation, SnapRange))
			continue;

		FVector EmitterDirection = CustomWorldDirection;
		
		if (DirectionType != ESandstormDirection::CUSTOM)
		{
			EmitterDirection = InitialSplineComponent->GetRightVectorAtDistanceAlongSpline(SplineQuantity, ESplineCoordinateSpace::World);
			
			if (DirectionType == ESandstormDirection::CCW)
				EmitterDirection = -EmitterDirection;
		}
		
		FTransform EmitterRelativeTransform = UKismetMathLibrary::MakeTransform(SnappedWorldPosition, UKismetMathLibrary::MakeRotFromX(EmitterDirection), FVector(EmitterScale));

		UActorComponent* AddedComponent = AddComponentByClass(UNiagaraComponent::StaticClass(), true, EmitterRelativeTransform, false);
		UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(AddedComponent);
		NiagaraComponent->SetAsset(NiagaraSystemAsset, false);
	}
}

bool ASandstorm::GetSnappedWorldPosition(FVector& SnappedWorldPosition, const FVector& WorldPosition, const float Range) const
{
	const FVector Start = FVector(WorldPosition.X, WorldPosition.Y, WorldPosition.Z + Range);
	const FVector End = FVector(WorldPosition.X, WorldPosition.Y, WorldPosition.Z - Range);

	FHitResult OutHit;
	
	const bool DidHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility);

	SnappedWorldPosition = OutHit.ImpactPoint;

	return DidHit;
}

bool ASandstorm::GetSnappedRelativePosition(FVector& SnappedRelativePosition, const FVector& WorldPosition, const float Range) const
{
	const bool DidHit = GetSnappedWorldPosition(SnappedRelativePosition, WorldPosition, Range);

	SnappedRelativePosition = GetActorTransform().InverseTransformPosition(SnappedRelativePosition);
	
	return DidHit; 
}

void ASandstorm::OnAdvancementUpdate(float Ratio)
{
	const int EmitterCount = NiagaraComponents.Num();

	const float TargetDistance = TargetSplineComponent->GetSplineLength() / static_cast<float>(EmitterCount);
	
	for (int i = 0; i < EmitterCount; i++)
	{
		const float InitialSplineQuantity = EmitterDistance * i;
		const float TargetSplineQuantity = TargetDistance * i;

		const FVector InitialWorldLocation = InitialSplineComponent->GetLocationAtDistanceAlongSpline(InitialSplineQuantity, ESplineCoordinateSpace::World);
		const FVector TargetWorldLocation = TargetSplineComponent->GetLocationAtDistanceAlongSpline(TargetSplineQuantity, ESplineCoordinateSpace::World);

		const FVector InterpolatedWorldLocation = UKismetMathLibrary::VLerp(InitialWorldLocation, TargetWorldLocation, Ratio);

		FVector SnappedWorldPosition;
		
		if (!GetSnappedWorldPosition(SnappedWorldPosition, InterpolatedWorldLocation, SnapRange))
			continue;

		FVector EmitterDirection = CustomWorldDirection;
		
		if (DirectionType != ESandstormDirection::CUSTOM)
		{
			const FVector InitialRight = InitialSplineComponent->GetLocationAtDistanceAlongSpline(InitialSplineQuantity, ESplineCoordinateSpace::World);
			const FVector TargetRight = TargetSplineComponent->GetLocationAtDistanceAlongSpline(TargetSplineQuantity, ESplineCoordinateSpace::World);

			EmitterDirection = UKismetMathLibrary::VLerp(InitialRight, TargetRight, Ratio);

			EmitterDirection = -UKismetMathLibrary::ProjectVectorOnToPlane(EmitterDirection, FVector::UpVector);

			if (DirectionType == ESandstormDirection::CCW)
				EmitterDirection = -EmitterDirection;
		}
		
		FTransform EmitterWorldTransform = UKismetMathLibrary::MakeTransform(SnappedWorldPosition, UKismetMathLibrary::MakeRotFromX(EmitterDirection), FVector(EmitterScale));

		NiagaraComponents[i]->SetWorldTransform(EmitterWorldTransform);
	}
}

void ASandstorm::OnAdvancementFinished()
{
	Destroy();
}
