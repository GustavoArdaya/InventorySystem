// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemPopUp.generated.h"

class USizeBox;
class UTextBlock;
class USlider;
class UButton;
/**
 * Item Pop-Up widget to display options when right clicking on item on Inventory Grid
 */
UCLASS()
class INVENTORY_API UInv_ItemPopUp : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

private:

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> Button_Split;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> Button_Drop;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> Button_Consume;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> Slider_Split;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SplitAmount;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UFUNCTION()
	void SplitButtonClicked();

	UFUNCTION()
	void DropButtonClicked();

	UFUNCTION()
	void ConsumeButtonClicked();

	UFUNCTION()
	void SliderValueChanged(float Value);
};
