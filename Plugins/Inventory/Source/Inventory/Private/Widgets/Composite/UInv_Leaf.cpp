// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/UInv_Leaf.h"

void UUInv_Leaf::ApplyFunction(FuncType Function)
{
	Function(this);
}
