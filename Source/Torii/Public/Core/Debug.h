// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Debug.generated.h"

#define ERROR 1
#define WARNING 2
#define LOG 3
#define SUCCESS 4
/**
 * 
 */
UCLASS()
class TORII_API UDebug : public UObject
{
	GENERATED_BODY()

public:
	static void Print(int type, FString s);
	static void PrintToScreen(int type, FString string, int displayForSeconds);
	static void AddDebugSphere(int type, FVector ActorLocation, float Radius);
	static void AddDebugBox(int type, FVector ActorLocation, FVector Scale3D);
	static FColor color(int type);
};
