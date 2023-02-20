#pragma once

#include "CoreMinimal.h"
#include "AmberCharacter.h"
#include "Gecko.generated.h"

UCLASS()
class AMBER_API AGecko : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	AGecko();

	class AAmberCharacter* player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SelfParameters")
	float minDistFromPlayer = 600.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelfParameters")
	float fleeDistance = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelfParameters")
	float timeBetweenRoam = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelfParameters")
	float roamRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelfParameters")
	float rotationSpeed = 4.f;

	UPROPERTY(BlueprintReadWrite, Category = "SelfParameters")
	bool isNearPlayer = false;

	UPROPERTY(BlueprintReadWrite, Category = "SelfParameters")
	bool hasReachFleeDistance = false;

	bool isHiding = false;
	bool isRotating = false;
	bool hasNewPos = false;

	float timer = 0.f;
	float rotationTimeline = 0.f;

	FNavLocation endPos;
	FRotator startRot;
	FRotator endRot;

	bool startRoamTimer = false;
	bool canRoam = false;
};