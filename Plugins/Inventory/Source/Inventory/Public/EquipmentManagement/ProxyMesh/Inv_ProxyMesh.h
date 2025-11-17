// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inv_ProxyMesh.generated.h"

class UInv_EquipmentComponent;

UCLASS()
class INVENTORY_API AInv_ProxyMesh : public AActor
{
	GENERATED_BODY()

public:
	AInv_ProxyMesh();

	USkeletalMeshComponent* GetMesh() const { return ProxyMesh.Get(); }

protected:
	virtual void BeginPlay() override;

private:

	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInv_EquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ProxyMesh;

	FTimerHandle TimerForNextTick;

	void DelayedInitializeOwner();
	void DelayedInitialization();
};
