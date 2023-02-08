#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GeckoAIController.generated.h"

UCLASS()
class AMBER_API AGeckoAIController : public AAIController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* behaviorTreeComponent;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* bTree;

	class UBlackboardComponent* blackboard;

public:
	AGeckoAIController(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
};