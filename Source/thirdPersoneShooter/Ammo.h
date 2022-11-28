// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONESHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()

public:
	AAmmo();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
	// override Item.h's SetItemProperties so we can set AmmoMesh properties
	virtual void SetItemProperties(EItemState State) override;

private:
	// mesh for the ammo pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Ammo, meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* AmmoMesh;

	// ammo type for the ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Ammo, meta=(AllowPrivateAccess="true"))
	EAmmoType AmmoType;

public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const {return AmmoMesh;}
	FORCEINLINE EAmmoType GetAmmoType() const {return AmmoType;}
	
};
