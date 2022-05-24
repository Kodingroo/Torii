//SurvivalGame Project - The Unreal C++ Survival Game Course - Copyright Reuben Ward 2020


#include "Widgets/InteractionWidget.h"
#include "Gameplay/Components//InteractionComponent.h"

void UInteractionWidget::UpdateInteractionWidget(class UInteractionComponent* InteractionComponent)
{
	OwningInteractionComponent = InteractionComponent;
	OnUpdateInteractionWidget();
}
