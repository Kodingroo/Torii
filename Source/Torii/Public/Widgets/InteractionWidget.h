/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class TORII_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateInteractionWidget(class UInteractionComponent* InteractionComponent);
	UPROPERTY(BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn))
	UInteractionComponent* OwningInteractionComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateInteractionWidget();
};
