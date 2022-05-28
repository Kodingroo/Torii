// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Torii_HUD.generated.h"

/**
 * 
 */
UCLASS()
class TORII_API ATorii_HUD : public AHUD
{
	GENERATED_BODY()

public:
	ATorii_HUD();

protected:
	TSharedPtr<class SK_MainMenuWidget> menuWidget;
	TSharedPtr<SWidget> menuWidgetContainer;

	TSharedPtr<class SK_HUD_SideScrollerWidget> HUDWidget;
	TSharedPtr<SWidget> HUDWidgetContainer;

	TSharedPtr<class SK_YouWinWidget> YouWinWidget;
	TSharedPtr<SWidget> YouWinWidgetContainer;
	
	virtual void BeginPlay();

public:
	void DisplayMenu();
	void RemoveMenu();
	void DisplayWinScreen();
	void RemoveWinScreen();
};
