/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToriiGameMode.generated.h"

class AMainPlayerController;
/**
 * 
 */
UCLASS()
class TORII_API AToriiGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AToriiGameMode();

	virtual void StartPlay() override;

	UFUNCTION()
	void OpenMenu();

	UFUNCTION(BlueprintCallable, Category="Initialize Level")
	void InitFeatherCollectedEvent() const;

	UFUNCTION(BlueprintCallable, Category="Collected Feathers")
	void DisplayCollectedFeathersWidget();
	void DisplayDoubleJumpWidget();

private:
	void SetupPlayer();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UUserWidget> MainMenuWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widgets")
	UUserWidget* MMW;

	UPROPERTY(VisibleAnywhere, Category="References")
	AMainPlayerController* MainPlayerController;
		
	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UUserWidget> CollectFeatherWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widgets")
	UUserWidget* CFW;

			
	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UUserWidget> DoubeJumpWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widgets")
	UUserWidget* DJW;
};
