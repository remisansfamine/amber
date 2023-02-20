#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Rotate.generated.h"

UCLASS()
class AMBER_API UBTTask_Rotate : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_Rotate(const FObjectInitializer& ObjectInitializer);	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AGecko* gecko;
};