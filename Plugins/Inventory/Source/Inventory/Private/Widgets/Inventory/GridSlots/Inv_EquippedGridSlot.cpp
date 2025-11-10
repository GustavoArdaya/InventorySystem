// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/Inv_EquippedGridSlot.h"

#include "Components/Image.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"

void UInv_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable()) return;
	UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetOccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInv_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable()) return;
	UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetUnoccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply UInv_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
	return FReply::Handled();
}

UInv_EquippedSlottedItem* UInv_EquippedGridSlot::OnItemEquipped(UInv_InventoryItem* Item,
	const FGameplayTag& InEquipmentTypeTag, float TileSize)
{
	// Check Valid EquipmentTypeTag

	// Get Grid Dimensions

	// Calculate draw size for EquippedSlottedItem
	// Create EquippedSlottedItem
	// Set Inventory Item
	// Set EquipmentTypeTag
	// Hide StackCount
	// Set Inventory Item on this class
	// Set Image Brush
	// Add Image to EquippedGridSlot
	// Add Slotted Item as Child of Widget overlay
	// Return EquippedSlottedItem
	return nullptr;
}
