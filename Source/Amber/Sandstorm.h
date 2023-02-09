// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Sandstorm.generated.h"

class USplineComponent;

UENUM(BlueprintType)
enum class ESandstormDirection : uint8 {
	CW = 0		UMETA(DisplayName = "Clockwise"),
	CCW = 1		UMETA(DisplayName = "Counter-clockwise"),
	CUSTOM = 2	UMETA(DisplayName = "Custom"),
};

UCLASS()
class AMBER_API ASandstorm : public AActor
{
	GENERATED_BODY()

	TArray<class UNiagaraComponent*> NiagaraComponents;

	FOnTimelineFloat AdvancementUpdatedEvent;
	FOnTimelineEvent AdvancementFinishedEvent;
public:	
	// Sets default values for this actor's properties
	ASandstorm();
	
	UPROPERTY(EditAnywhere)
	float EmitterScale = 1.f;

	UPROPERTY(EditAnywhere)
	UCurveFloat* AdvancementCurve = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTimelineComponent* AdvancementTimelineComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESandstormDirection DirectionType = ESandstormDirection::CW;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CustomWorldDirection = FVector::ForwardVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EmitterDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SnapRange = 100000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USplineComponent* InitialSplineComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USplineComponent* TargetSplineComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* NiagaraSystemAsset = nullptr;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool GetSnappedWorldPosition(FVector& SnappedWorldPosition, const FVector& WorldPosition, const float Range) const;

	UFUNCTION(BlueprintCallable)
	bool GetSnappedRelativePosition(FVector& SnappedRelativePosition, const FVector& WorldPosition, const float Range) const;

	UFUNCTION()
	void OnAdvancementUpdate(float Ratio);

	UFUNCTION()
	void OnAdvancementFinished();
};
