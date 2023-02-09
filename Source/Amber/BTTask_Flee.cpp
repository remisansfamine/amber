#include "BTTask_Flee.h"
#include "AIController.h"
#include "Gecko.h"

UBTTask_Flee::UBTTask_Flee(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Flee");
}

EBTNodeResult::Type UBTTask_Flee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* controller = OwnerComp.GetAIOwner();
	if (gecko == nullptr)
		gecko = Cast<AGecko>(controller->GetPawn());

	FVector location = gecko->GetActorLocation();
	location.X += 1000.f;

	controller->MoveToLocation(location, 1.f, false);	

	return EBTNodeResult::Succeeded;
}
