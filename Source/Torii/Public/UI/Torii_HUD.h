/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

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

	void DisplayMenu();
	void RemoveMenu();
	void DisplayWinScreen();
	void RemoveWinScreen();
	
protected:
	virtual void BeginPlay();

	TSharedPtr<class SK_MainMenuWidget> menuWidget;
	TSharedPtr<SWidget> menuWidgetContainer;

	TSharedPtr<class SK_HUD_SideScrollerWidget> HUDWidget;
	TSharedPtr<SWidget> HUDWidgetContainer;

	TSharedPtr<class SK_YouWinWidget> YouWinWidget;
	TSharedPtr<SWidget> YouWinWidgetContainer;
};
