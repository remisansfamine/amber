#include "BTTask_Roam.h"

#include "AIController.h"
#include "Gecko.h"

UBTTask_Roam::UBTTask_Roam(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Roam");
}

EBTNodeResult::Type UBTTask_Roam::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* controller = OwnerComp.GetAIOwner();
	if (gecko == nullptr)
		gecko = Cast<AGecko>(controller->GetPawn());

	controller->MoveToLocation(gecko->endPos.Location, 1.f, false);

	gecko->hasNewPos = false;
	gecko->timer = 0.f;
	gecko->canRoam = false;
	gecko->startRoamTimer = false;
	
	return EBTNodeResult::Succeeded;
}