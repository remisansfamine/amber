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

	gecko->isNearPlayer = true;
	
	FVector geckoLoc = gecko->GetActorLocation();
	FVector playerDir = geckoLoc - gecko->player->GetActorLocation();
	playerDir.Normalize();
	geckoLoc += playerDir * gecko->fleeDistance;

	controller->MoveToLocation(geckoLoc, 1.f, false);	

	return EBTNodeResult::Succeeded;
}
