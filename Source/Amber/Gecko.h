#pragma once

#include "CoreMinimal.h"
#include "AmberCharacter.h"
#include "GameFramework/Character.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float fleeDistance = 600.f;
	
	void Flee();
};