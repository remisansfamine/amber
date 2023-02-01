// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quicksands.generated.h"

UCLASS()
class AMBER_API AQuicksands : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CubeMeshComponent = nullptr;

	UPROPERTY()
	ACharacter* Prey = nullptr;

	float InitialWalkSpeed = 0.f;
	float ImpactPointHeight = 0.f;
	
public:	
	// Sets default values for this actor's properties
	AQuicksands();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void ComputePreyStats(); 

public:
	UPROPERTY(EditAnywhere)
	float ExpMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere)
	float MinGravityScale = 0.f;

	UPROPERTY(EditAnywhere)
	float MinMaxSpeedScale = 0.25f;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
