// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UInv_Leaf.h"
#include "Inv_Leaf_Image.generated.h"

class USizeBox;
class UImage;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_Leaf_Image : public UUInv_Leaf
{
	GENERATED_BODY()

public:

	void SetImage(UTexture2D* Texture) const;
	void SetBoxSize(const FVector2D& Size) const;
	void SetImageSize(const FVector2D& Size) const;
	FVector2D GetImageSize() const;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Icon;
};
