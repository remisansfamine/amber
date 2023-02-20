#include "BTDecorator_HasNewPos.h"

#include "Gecko.h"
#include "GeckoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_HasNewPos::UBTDecorator_HasNewPos(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Has New Pos");
}

bool UBTDecorator_HasNewPos::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const 
{
	const AAIController* controller = Cast<AGeckoAIController>(OwnerComp.GetAIOwner());
	const AGecko* gecko = Cast<AGecko>(controller->GetPawn());

	return gecko->hasNewPos;
}