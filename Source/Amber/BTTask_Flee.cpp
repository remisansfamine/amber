#include "BTTask_Flee.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Gecko.h"

UBTTask_Flee::UBTTask_Flee(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Flee");
}

EBTNodeResult::Type UBTTask_Flee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* actualGecko = OwnerComp.GetAIOwner();
	if (gecko == nullptr)
		gecko = Cast<AGecko>(actualGecko->GetPawn());

	gecko->Flee();

	return EBTNodeResult::Succeeded;
}
