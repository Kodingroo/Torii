/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "MainPlayerController.generated.h"

class AToriiGameMode;
/**
 * 
 */
UCLASS()
class TORII_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainPlayerController();
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="HUD")
	void OpenMenu();
	UFUNCTION(BlueprintCallable, Category="Feathers")
	void BindFeatherCollectedEvent();
	UFUNCTION(BlueprintCallable, Category="Feathers")
	void ObserveFeatherCollected(bool bCoinCollected);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feather Properties")
	int TotalFeathers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feather Properties")
	int CollectedFeathers;


	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UUserWidget> MainMenuWidget;
	UPROPERTY(EditAnywhere, Category="Class References")
	TSubclassOf<AActor> FeatherClassRef;
	UPROPERTY(VisibleAnywhere, Category="Coin Array")
	TArray<AActor*> FoundFeathersInLevel;

	UPROPERTY(VisibleAnywhere, Category="References")
	AToriiGameMode* GameModeRef;
};
