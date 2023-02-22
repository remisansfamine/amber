// Fill out your copyright notice in the Description page of Project Settings.


#include "FogComponent.h"

UFogComponent::UFogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFogComponent::BeginPlay()
{
	Super::BeginPlay();
	
	FogSystemComponent = Cast<UNiagaraComponent>(GetOwner()->GetComponentByClass(UNiagaraComponent::StaticClass()));
}

