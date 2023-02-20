#include "BTTask_Rotate.h"

#include "AIController.h"
#include "Gecko.h"

UBTTask_Rotate::UBTTask_Rotate(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Rotate");
}

EBTNodeResult::Type UBTTask_Rotate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* controller = OwnerComp.GetAIOwner();
	if (gecko == nullptr)
		gecko = Cast<AGecko>(controller->GetPawn());
	
	gecko->rotationTimeline += 1 / gecko->rotationSpeed;
	
	FRotator currRot = FRotator(FQuat::Slerp(gecko->startRot.Quaternion(), gecko->endRot.Quaternion(), gecko->rotationTimeline));
	FRotator newRot(0.f, currRot.Yaw, 0.f);

	gecko->SetActorRotation(newRot);

	if (gecko->rotationTimeline > 1.f)
	{
		gecko->isRotating = false;
		gecko->rotationTimeline = 0.f;
		gecko->canRoam = true;
	}

	return EBTNodeResult::Succeeded;
}