#include "GeckoAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	blackboard->SetValueAsFloat(TEXT("minDistFromPlayer"), gecko->minDistFromPlayer);
	blackboard->SetValueAsFloat(TEXT("timeBetweenRoam"), gecko->timeBetweenRoam);
	blackboard->SetValueAsBool(TEXT("isHiding"), gecko->isHiding);
	blackboard->SetValueAsBool(TEXT("isRotating"), gecko->isRotating);
	blackboard->SetValueAsBool(TEXT("hasNewPos"), gecko->hasNewPos);
	blackboard->SetValueAsBool(TEXT("canRoam"), gecko->canRoam);
}