// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Debug.h"
#include "DrawDebugHelpers.h"

void UDebug::Print(int type, FString s)
{;
	switch(type) {
	case ERROR:
		UE_LOG(LogTemp, Error, TEXT("%s"), *s);
		break;
	case WARNING:
		UE_LOG(LogTemp, Warning, TEXT("%s"), *s);
		break;
	case LOG:
		UE_LOG(LogTemp, Log, TEXT("%s"), *s);
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Error in UDebug::print. Switch failing."));
		break;
	}
}

void UDebug::PrintToScreen(int type, FString string, int displayForSeconds)
{
	GEngine->AddOnScreenDebugMessage(1, displayForSeconds, color(type), *string, true, FVector2D(1.f,1.f));
}

void UDebug::AddDebugSphere(int type, FVector ActorLocation, float Radius)
{
	const UWorld* World = GEngine->GameViewport->GetWorld();
    
	DrawDebugSphere(World, ActorLocation, Radius, 10, color(type), false, 0);
}

void UDebug::AddDebugBox(int type, FVector ActorLocation, FVector Scale3D)
{
	const UWorld* World = GEngine->GameViewport->GetWorld();
    
	DrawDebugBox(World, ActorLocation, Scale3D, color(type), true, 0);
}

FColor UDebug::color(int type)
{
	FColor color;
	switch(type) {
	case ERROR:
		color = FColor::Red;
		break;
	case WARNING:
		color = FColor::Yellow;
		break;
	case LOG:
		color = FColor::Blue;
		break;
	case SUCCESS:
		color = FColor::Green;
		break;
	default:
		color = FColor::Cyan;
		UE_LOG(LogTemp, Error, TEXT("Error in UDebug::color. Switch failing."));
		break;
	}
	return color;
}