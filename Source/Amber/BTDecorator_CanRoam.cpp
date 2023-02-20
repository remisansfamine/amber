#include "BTDecorator_CanRoam.h"

#include "Gecko.h"
#include "GeckoAIController.h"

UBTDecorator_CanRoam::UBTDecorator_CanRoam(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Can Roam");
}

bool UBTDecorator_CanRoam::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const 
{
	const AAIController* controller = Cast<AGeckoAIController>(OwnerComp.GetAIOwner());
	const AGecko* gecko = Cast<AGecko>(controller->GetPawn());

	return gecko->canRoam;
}