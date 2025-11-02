#include "Items/Fragments/Inv_ItemFragment.h"

void FInv_HealthPotionFragment::OnConsume(APlayerController* PlayerController)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
			FString::Printf(TEXT("Health Potion consumed! Healing by: %f"), HealAMount));
	}
}

void FInv_ManaPotionFragment::OnConsume(APlayerController* PlayerController)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue,
			FString::Printf(TEXT("Mana Potion consumed! Regenerating mana by: %f"), ManaAMount));
	}
}
