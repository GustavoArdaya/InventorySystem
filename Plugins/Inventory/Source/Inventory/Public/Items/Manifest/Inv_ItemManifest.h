#pragma once

#include "CoreMinimal.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ItemManifest.generated.h"

/**
 * Item Manifest contains all necessary data for creating new inventory item
**/

class UInv_InventoryItem;

USTRUCT(BlueprintType)
struct INVENTORY_API FInv_ItemManifest
{
	GENERATED_BODY()

	UInv_InventoryItem* Manifest(UObject* NewOuter);
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory{EInv_ItemCategory::None};
	
};


