#include "Gecko.h"
#include "AmberCharacter.h"
#include "Kismet/GameplayStatics.h"

AGecko::AGecko()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGecko::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AAmberCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AGecko::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (startRoamTimer)
		timer += DeltaTime;
}