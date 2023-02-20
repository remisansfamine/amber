#include "BTDecorator_IsRotating.h"

#include "Gecko.h"
#include "GeckoAIController.h"

UBTDecorator_IsRotating::UBTDecorator_IsRotating(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Is Rotating");
}

bool UBTDecorator_IsRotating::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const 
{
	const AAIController* controller = Cast<AGeckoAIController>(OwnerComp.GetAIOwner());
	const AGecko* gecko = Cast<AGecko>(controller->GetPawn());
	
	return gecko->isRotating;
}