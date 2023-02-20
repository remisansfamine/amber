// Fill out your copyright notice in the Description page of Project Settings.


#include "Worm.h"

#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
AWorm::AWorm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

}

void AWorm::ComputeEnvironmentIntersections(TArray<FHitResult>& OutHits, float AngleRate, int RayCount)
{
	const FVector WorldStart = GetActorTransform().TransformPosition(Radius * FVector::UpVector);
	FVector WorldLeftStart = WorldStart, WorldRightStart = WorldStart;

	float Cos = 1.f, Sin = 0.f, Angle = 0.f;
	for (int i = 0; i < RayCount; i++)
	{
		Angle += AngleRate;
		
		FMath::SinCos(&Cos, &Sin, Angle);

		const FVector LocalUp = Radius * Cos * FVector::UpVector;
		const FVector LocalLeftEnd = Radius * Sin * FVector::LeftVector + LocalUp;
		const FVector LocalRightEnd = Radius * Sin * FVector::RightVector + LocalUp;

		const FVector WorldLeftEnd = GetActorTransform().TransformPosition(LocalLeftEnd);
		const FVector WorldRightEnd = GetActorTransform().TransformPosition(LocalRightEnd);

        {
        	FHitResult OutHit;
        	if (GetWorld()->LineTraceSingleByChannel(OutHit, WorldLeftStart, WorldLeftEnd, ECollisionChannel::ECC_Visibility))
        		OutHits.Add(OutHit);
        
        	if (GetWorld()->LineTraceSingleByChannel(OutHit, WorldRightStart, WorldRightEnd, ECollisionChannel::ECC_Visibility))
        		OutHits.Add(OutHit);
        }

		WorldLeftStart = WorldLeftEnd;
		WorldRightStart = WorldRightEnd;
	}
}

void AWorm::AddEmitter(const FTransform& EmitterTransform)
{
	UActorComponent* AddedComponent = AddComponentByClass(UNiagaraComponent::StaticClass(), true, EmitterTransform, false);
	UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(AddedComponent);
	NiagaraComponent->SetAsset(NiagaraSystemAsset, false);
}

void AWorm::OnConstruction(const FTransform& Transform)
{
	float AngleRate = UKismetMathLibrary::GetPI() * 2.f / PolygonSideCount;

	TArray<FHitResult> OutHits;

	ComputeEnvironmentIntersections(OutHits, AngleRate, PolygonSideCount * 0.5f);

	for (int i = 0; i < OutHits.Num(); i++)
		AddEmitter(UKismetMathLibrary::MakeTransform(OutHits[i].ImpactPoint, FRotator::ZeroRotator, FVector::OneVector));
}
