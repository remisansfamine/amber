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

void AWorm::OnConstruction(const FTransform& Transform)
{
	float AngleRate = UKismetMathLibrary::GetPI() * 2.f / PolygonSideCount;
	
	float Cos = 0.f, Sin = 1.f;

	FVector WorldStart = GetActorTransform().TransformPosition((FVector::UpVector * Cos + FVector::RightVector * Sin) * Radius);

	float Angle = 0.f;
	for (int i = 0; i < PolygonSideCount; i++)
	{
		Angle += AngleRate;
		
		FMath::SinCos(&Cos, &Sin, Angle);
		const FVector LocalEnd = Radius * (Cos * FVector::UpVector + Sin * FVector::RightVector);
		const FVector WorldEnd = GetActorTransform().TransformPosition(LocalEnd);

		FHitResult OutHit;
		if (GetWorld()->LineTraceSingleByChannel(OutHit, WorldStart, WorldEnd, ECollisionChannel::ECC_Visibility))
		{
			FTransform EmitterTransform = UKismetMathLibrary::MakeTransform(OutHit.ImpactPoint, FRotator::ZeroRotator, FVector::OneVector);
			UActorComponent* AddedComponent = AddComponentByClass(UNiagaraComponent::StaticClass(), true, EmitterTransform, false);
			UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(AddedComponent);
			NiagaraComponent->SetAsset(NiagaraSystemAsset, false);
		}

		WorldStart = WorldEnd;
	}
}
