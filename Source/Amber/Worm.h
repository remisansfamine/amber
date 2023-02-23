// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Worm.generated.h"

UCLASS()
class AMBER_API AWorm : public AActor
{
	GENERATED_BODY()
	
	void AddEmitter(const FTransform& EmitterTransform);
	void AddCollider(const FTransform& ColliderTransform);
	void ComputeEnvironmentIntersections(TArray<FHitResult>& OutHits, float AngleRate, int RayCount);

public:	
	// Sets default values for this actor's properties
	AWorm();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* MeshComponent = nullptr;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere)
	float TorusThickness = 340.f;
	
	UPROPERTY(EditAnywhere)
	float TorusRadius = 1345.f;

	UPROPERTY(EditAnywhere)
	int PolygonSideCount = 15;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* NiagaraSystemAsset = nullptr;
};
