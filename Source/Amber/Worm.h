// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Worm.generated.h"

UCLASS()
class AMBER_API AWorm : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorm();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* MeshComponent = nullptr;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere)
	float Radius = 100.f;

	UPROPERTY(EditAnywhere)
	int PolygonSideCount = 4;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* NiagaraSystemAsset = nullptr;
};
