#include "Gecko.h"

#include "GeckoAIController.h"
#include "AmberCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/CharacterMovementComponent.h"

AGecko::AGecko()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGecko::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AAmberCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AGecko::Flee()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "FLEE");
}