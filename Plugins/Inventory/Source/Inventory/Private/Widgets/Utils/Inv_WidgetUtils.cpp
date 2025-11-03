// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Utils/Inv_WidgetUtils.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Widget.h"

FVector2D UInv_WidgetUtils::GetWidgetPosition(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	FVector2D PixelPosition;
	FVector2D ViewPortPosition;

	USlateBlueprintLibrary::LocalToViewport(Widget, Geometry, USlateBlueprintLibrary::GetLocalTopLeft(Geometry), PixelPosition, ViewPortPosition);
	return ViewPortPosition;
}

FVector2D UInv_WidgetUtils::GetWidgetSize(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize();
}

bool UInv_WidgetUtils::IsWithinBounds(const FVector2D& BoundaryPosition, const FVector2D& WidgetSize,
                                      const FVector2D& MousePosition)
{
	return MousePosition.X >= BoundaryPosition.X && MousePosition.X <= (BoundaryPosition.X + WidgetSize.X) &&
		MousePosition.Y >= BoundaryPosition.Y && MousePosition.Y <= (BoundaryPosition.Y + WidgetSize.Y);
}

FVector2D UInv_WidgetUtils::GetClampedWidgetPosition(const FVector2D& Boundary, const FVector2D& WidgetSize,
	const FVector2D& MousePosition)
{
	FVector2D ClampedPosition = MousePosition;

	// Adjust horizontal position to ensure widget within boundary
	if (MousePosition.X + WidgetSize.X > Boundary.X) //Exceeds right edge
	{
		ClampedPosition.X = Boundary.X - WidgetSize.X;
	}
	if (MousePosition.X < 0) // Widget Exceeds left edge
	{
		ClampedPosition.X = 0.f;
	}

	// Adjust Vertical position
	if (MousePosition.Y + WidgetSize.Y > Boundary.Y)
	{
		ClampedPosition.Y = Boundary.Y - WidgetSize.Y;
	}
	if (MousePosition.Y < 0)
	{
		ClampedPosition.Y = 0.f;
	}
	
	return ClampedPosition;
}

int32 UInv_WidgetUtils::GetIndexFromPosition(const FIntPoint& Position, int32 Columns)
{
	return Position.X + Position.Y * Columns;	
}

FIntPoint UInv_WidgetUtils::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}
