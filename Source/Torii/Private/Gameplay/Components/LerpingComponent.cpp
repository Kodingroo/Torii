// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/LerpingComponent.h"

ULerpingComponent::ULerpingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Parent = GetOwner();

	bShouldLoop = true;
	
	/* Variables for Sin */ 
	RunningTime = 0;

	/* Variables for True Lerp */ 
	LerpDuration = 3;
	WaitTime = 2;
}

void ULerpingComponent::BeginPlay()
{
	Super::BeginPlay();

	/* Set variables for Sin */ 
	BaseZLocation = Parent->GetActorLocation().Z;
	Amplitude = 150.f;
	Period = 2.f;
	PhaseShift = 1.f;
	VerticalShift = 1.f;
	
	/* Set variables for True Lerp */ 
	StartLocation = Parent->GetActorLocation();
	TargetLocation = StartLocation + FVector(0.f, 0.f, 600.f);
}

void ULerpingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/* Small delay just to stop it looking choppy due to lag on load */
	if (WaitTime > 0)
	{
		WaitTime -= DeltaTime;
		return;
	}

	if (bShouldLoop)
	{
		FVector NewLocation = Parent->GetActorLocation();

		NewLocation.Z = BaseZLocation + (Amplitude) * FMath::Sin(Period * RunningTime - PhaseShift) + VerticalShift;
		
		Parent->SetActorLocation(NewLocation);
		RunningTime += DeltaTime;
	}
	else
	{
		if (TimeElapsed < LerpDuration)
		{
			/* Move Parent Actor to Target Location */
			Parent->SetActorLocation(FMath::Lerp(StartLocation, TargetLocation, TimeElapsed / LerpDuration));
			/* Kep track of elapsed time */
			TimeElapsed += DeltaTime;
		}
	}
}

