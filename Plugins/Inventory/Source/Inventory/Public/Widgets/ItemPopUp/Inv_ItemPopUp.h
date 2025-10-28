// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemPopUp.generated.h"


class USizeBox;
class UTextBlock;
class USlider;
class UButton;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPopUpMenuSplit, int32, SplitAmount, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuDrop, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuConsume, int32, Index);


/**
 * Item Pop-Up widget to display options when right clicking on item on Inventory Grid
 */
UCLASS()
class INVENTORY_API UInv_ItemPopUp : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;

	FPopUpMenuSplit OnSplit;
	FPopUpMenuDrop OnDrop;
	FPopUpMenuConsume OnConsume;

	int32 GetSplitAmount() const;

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

	int32 GridIndex{INDEX_NONE};

	UFUNCTION()
	void SplitButtonClicked();

	UFUNCTION()
	void DropButtonClicked();

	UFUNCTION()
	void ConsumeButtonClicked();

	UFUNCTION()
	void SliderValueChanged(float Value);
};
