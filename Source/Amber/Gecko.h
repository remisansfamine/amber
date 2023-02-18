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

public:
	AGecko();

	class AAmberCharacter* player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SelfParameters")
	float minDistFromPlayer = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelfParameters")
	bool isNearPlayer = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelfParameters")
	bool hasReachFleeDistance = false;
};