#include "BTTask_GetNewPos.h"

#include "AIController.h"
#include "Gecko.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_GetNewPos::UBTTask_GetNewPos(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Get New Pos");
}

EBTNodeResult::Type UBTTask_GetNewPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* controller = OwnerComp.GetAIOwner();
	if (gecko == nullptr)
		gecko = Cast<AGecko>(controller->GetPawn());

	FVector startPos = gecko->GetActorLocation();
	gecko->startRot = gecko->GetActorRotation();
	gecko->endPos = FNavLocation(startPos);
	
	UNavigationSystemV1* navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (navSystem->GetRandomReachablePointInRadius(startPos, gecko->roamRadius, gecko->endPos))
	{
		gecko->endRot = UKismetMathLibrary::FindLookAtRotation(startPos, gecko->endPos);
		
		gecko->isRotating = true;
		gecko->hasNewPos = true;

		gecko->startRoamTimer = true;
		
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}