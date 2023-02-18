#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Hide.generated.h"

UCLASS()
class AMBER_API UBTTask_Hide : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_Hide(const FObjectInitializer& ObjectInitializer);	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AGecko* gecko;
};
