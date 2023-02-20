#include "BTDecorator_HasWaited.h"

#include "Gecko.h"
#include "GeckoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_HasWaited::UBTDecorator_HasWaited(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Has Waited");
}

bool UBTDecorator_HasWaited::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const 
{
	const AAIController* controller = Cast<AGeckoAIController>(OwnerComp.GetAIOwner());
	const UBlackboardComponent* blackboard = controller->GetBlackboardComponent();

	const AGecko* gecko = Cast<AGecko>(controller->GetPawn());

	const float timeBetweenRoam = blackboard->GetValueAsFloat("timeBetweenRoam");
	
	return gecko->timer >= timeBetweenRoam;
}