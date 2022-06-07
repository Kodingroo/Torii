/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#include "Gameplay/LerpingPlatform.h"


ALerpingPlatform::ALerpingPlatform()
{
	PrimaryActorTick.bCanEverTick = false;

	PaperSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	PaperSprite->SetupAttachment(RootComponent);
	
}

void ALerpingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	AttachLerpingComponent();
}

void ALerpingPlatform::AttachLerpingComponent()
{
	LerpingComponent = NewObject<ULerpingComponent>(this, ULerpingComponent::StaticClass(), TEXT("Lerping Component"));

	if (LerpingComponent)
	{
		LerpingComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		LerpingComponent->RegisterComponentWithWorld(GetWorld());
		LerpingComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
}