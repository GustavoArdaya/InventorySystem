// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_SpatialInventory.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Widgets/Inventory/GridSlots/Inv_EquippedGridSlot.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_EquippedSlottedItem.h"
#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "Widgets/ItemDescription/Inv_ItemDescription.h"

void UInv_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equippables->OnClicked.AddDynamic(this, &ThisClass::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);

	Grid_Equippables->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);

	ShowEquippables();

	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		UInv_EquippedGridSlot* EquippedGridSlot = Cast<UInv_EquippedGridSlot>(Widget);
		if (IsValid(EquippedGridSlot))
		{
			EquippedGridSlots.Add(EquippedGridSlot);
			EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this, &ThisClass::EquippedGridSlotClicked);
		}
	});
}

void UInv_SpatialInventory::EquippedGridSlotClicked(UInv_EquippedGridSlot* GridSlot,
	const FGameplayTag& EquipmentTypeTag)
{
	// Check if we can Equip Hover Item
	if (!CanEquipHoverItem(GridSlot, EquipmentTypeTag)) return;
	UInv_HoverItem* HoverItem = GetHoverItem();
	
	// Create and add equipped slotted item
	const float TileSize = UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize();
	UInv_EquippedSlottedItem* EquippedSlottedItem =  GridSlot->OnItemEquipped(
		HoverItem->GetInventoryItem(), EquipmentTypeTag, TileSize);
	
	EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);

	// Clear Hover Item
	Grid_Equippables->ClearHoverItem();

	// Inform server item equipped/unequipped
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	InventoryComponent->Server_EquipSlotClicked(HoverItem->GetInventoryItem());

	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(HoverItem->GetInventoryItem());
	}	
}

void UInv_SpatialInventory::EquippedSlottedItemClicked(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	// Remove Item Description
	UInv_InventoryStatics::ItemUnhovered(GetOwningPlayer());

	if (IsValid(GetHoverItem()) && ( GetHoverItem())->IsStackable()) return;
	
	// Get Item To equip
	UInv_InventoryItem* ItemToEquip = IsValid(GetHoverItem()) ? GetHoverItem()->GetInventoryItem() : nullptr;
	
	// Get Item to Unequip
	UInv_InventoryItem* ItemToUnequip = EquippedSlottedItem->GetInventoryItem();
	
	// Get Equipped Grid Slot
	UInv_EquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);
	
	// Clear Equipped Slot (null Item)
	ClearSlotOfItem(EquippedGridSlot);
	
	// Assign previously equipped item as Hover Item
	Grid_Equippables->AssignHoverItem(ItemToUnequip);
	
	// Remove Equipped Slotted Item From GridSlot
	RemoveEquippedSlottedItem(EquippedSlottedItem);
	
	// Make New equipped slotted item for new Hover Item
	MakeEquippedSlottedItem(EquippedSlottedItem, EquippedGridSlot, ItemToEquip);
	
	// Broadcast delegates from InventoryComponent
	BroadcastSlotClickedDelegates(ItemToEquip, ItemToUnequip);
}

FReply UInv_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();	
}

void UInv_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(ItemDescription)) return;
	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);
}

void UInv_SpatialInventory::SetItemDescriptionSizeAndPosition(UInv_ItemDescription* Description,
	UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCPS)) return;

	const FVector2D ItemDescriptionSize = Description->GetBoxSize();
	ItemDescriptionCPS->SetSize(ItemDescriptionSize);

	FVector2D ClampedPosition = UInv_WidgetUtils::GetClampedWidgetPosition(
		UInv_WidgetUtils::GetWidgetSize(Canvas),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer()));

	ItemDescriptionCPS->SetPosition(FVector2D(ClampedPosition));
}

bool UInv_SpatialInventory::CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot,
	const FGameplayTag& EquipmentTypeTag) const
{
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid()) return false;

	UInv_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;

	UInv_InventoryItem* HeldItem = HoverItem->GetInventoryItem();

	return HasHoverItem()
		&& IsValid(HeldItem)
		&& !HoverItem->IsStackable()
		&& HeldItem->GetItemManifest().GetItemCategory() == EInv_ItemCategory::Equippable
		&& HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag); 
	
}

UInv_EquippedGridSlot* UInv_SpatialInventory::FindSlotWithEquippedItem(UInv_InventoryItem* EquippedItem) const
{
	auto* FoundEquippedGridSlot = EquippedGridSlots.FindByPredicate(
		[EquippedItem](const UInv_EquippedGridSlot* GridSlot)
		{
			return GridSlot->GetInventoryItem() == EquippedItem;
		});
	return FoundEquippedGridSlot ? *FoundEquippedGridSlot : nullptr;
}

void UInv_SpatialInventory::ClearSlotOfItem(UInv_EquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		EquippedGridSlot->SetEquippedSlottedItem(nullptr);
		EquippedGridSlot->SetInventoryItem(nullptr);
	}
}

void UInv_SpatialInventory::RemoveEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	if (!IsValid(EquippedSlottedItem)) return;

	if (EquippedSlottedItem->OnEquippedSlottedItemClicked.IsAlreadyBound(this, &ThisClass::EquippedSlottedItemClicked))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.RemoveDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}
	EquippedSlottedItem->RemoveFromParent();
}

void UInv_SpatialInventory::MakeEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem,
	UInv_EquippedGridSlot* EquippedGridSlot, UInv_InventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot)) return;
	UInv_EquippedSlottedItem* NewEquippedSlottedItem =  EquippedGridSlot->OnItemEquipped(
		ItemToEquip,
		EquippedSlottedItem->GetEquipmentTypeTag(),
		UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize());

	if (IsValid(NewEquippedSlottedItem))
	{
		NewEquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}

	EquippedGridSlot->SetEquippedSlottedItem(NewEquippedSlottedItem);
}

void UInv_SpatialInventory::BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip,
	UInv_InventoryItem* ItemToUnequip) const
{
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));
	InventoryComponent->Server_EquipSlotClicked(ItemToEquip, ItemToUnequip);

	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{		
		InventoryComponent->OnItemEquipped.Broadcast(ItemToEquip);		
		InventoryComponent->OnItemUnequipped.Broadcast(ItemToUnequip);		
	}
}

FInv_SlotAvailabilityResult UInv_SpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComponent) const
{
	switch (UInv_InventoryStatics::GetItemCategoryFromItemComp(ItemComponent))
	{
		case EInv_ItemCategory::Equippable:
			return Grid_Equippables->HasRoomForItem(ItemComponent);
		case EInv_ItemCategory::Consumable:
			return Grid_Consumables->HasRoomForItem(ItemComponent);
		case EInv_ItemCategory::Craftable:
			return Grid_Craftables->HasRoomForItem(ItemComponent);
		default:
			UE_LOG(LogInventory, Error, TEXT("ItemComponent has invalid category"));
			return FInv_SlotAvailabilityResult();
	}
}

void UInv_SpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	UInv_ItemDescription* DescriptionWidget = GetItemDescription();
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this, &Manifest, DescriptionWidget]()
	{
		Manifest.AssimilateInventoryFragment(DescriptionWidget);
		GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
	});

	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimer, DescriptionTimerDelegate, DescriptionTimerDelay, false);
}

void UInv_SpatialInventory::OnItemUnhovered()
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
}

bool UInv_SpatialInventory::HasHoverItem() const
{
	if (Grid_Equippables->HasHoverItem()) return true;
	if (Grid_Consumables->HasHoverItem()) return true;
	if (Grid_Craftables->HasHoverItem()) return true;
	return false;
}

UInv_HoverItem* UInv_SpatialInventory::GetHoverItem() const
{
	if (!ActiveGrid.IsValid()) return nullptr;
	return ActiveGrid->GetHoverItem();
}

float UInv_SpatialInventory::GetTileSize() const
{
	return Grid_Equippables->GetTileSize();
}

UInv_ItemDescription* UInv_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UInv_ItemDescription>(GetOwningPlayer()->GetWorld(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}

void UInv_SpatialInventory::ShowEquippables()
{
	SetActiveGrid(Grid_Equippables, Button_Equippables);
}

void UInv_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables, Button_Consumables);
}

void UInv_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables, Button_Craftables);
}

void UInv_SpatialInventory::DisableButton(UButton* Button)
{
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->HideCursor();
		ActiveGrid->OnHide();
	} 
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid()) ActiveGrid->ShowCursor();
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}


