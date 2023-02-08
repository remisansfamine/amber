#include "GeckoAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "Gecko.h"

AGeckoAIController::AGeckoAIController(FObjectInitializer const& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("/Game/AI/Gecko_BT.Gecko_BT"));

	if (obj.Succeeded())
		bTree = obj.Object;

	behaviorTreeComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("bTreeComponent"));
	blackboard = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComponent"));

	SetActorTickEnabled(false);
}

void AGeckoAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(bTree);
	behaviorTreeComponent->StartTree(*bTree, EBTExecutionMode::Looped);
	AGecko* gecko = Cast<AGecko>(GetPawn());
	blackboard->SetValueAsObject(TEXT("SelfActor"), gecko);
	blackboard->SetValueAsFloat(TEXT("fleeDistance"), gecko->fleeDistance);
}