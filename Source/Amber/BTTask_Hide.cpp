#include "BTTask_Hide.h"
#include "AIController.h"
#include "Gecko.h"

UBTTask_Hide::UBTTask_Hide(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Hide");
}

EBTNodeResult::Type UBTTask_Hide::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* controller = OwnerComp.GetAIOwner();
	if (gecko == nullptr)
		gecko = Cast<AGecko>(controller->GetPawn());

	gecko->hasReachFleeDistance = true;
	
	return EBTNodeResult::Succeeded;
}
