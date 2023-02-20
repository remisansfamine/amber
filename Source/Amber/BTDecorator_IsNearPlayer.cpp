#include "BTDecorator_IsNearPlayer.h"

#include "Gecko.h"
#include "GeckoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsNearPlayer::UBTDecorator_IsNearPlayer(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Is Near Player");
}

bool UBTDecorator_IsNearPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const 
{
	const AAIController* controller = Cast<AGeckoAIController>(OwnerComp.GetAIOwner());
	const UBlackboardComponent* blackboard = controller->GetBlackboardComponent();

	const AGecko* gecko = Cast<AGecko>(controller->GetPawn());

	const float minDistFromPlayer = blackboard->GetValueAsFloat("minDistFromPlayer");
	const float distanceFromPlayer = FVector::Dist(gecko->GetActorLocation(), gecko->player->GetActorLocation());

	return distanceFromPlayer <= minDistFromPlayer;
}