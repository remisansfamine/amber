#include "BTDecorator_isHiding.h"

#include "Gecko.h"
#include "GeckoAIController.h"

UBTDecorator_IsHiding::UBTDecorator_IsHiding(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Is Hiding");
}

bool UBTDecorator_IsHiding::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const 
{
	const AAIController* controller = Cast<AGeckoAIController>(OwnerComp.GetAIOwner());
	const AGecko* gecko = Cast<AGecko>(controller->GetPawn());
	
	return gecko->isHiding;
}