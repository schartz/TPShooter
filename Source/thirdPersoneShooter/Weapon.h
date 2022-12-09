// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Engine/DataTable.h"
#include "WeaponType.h"
#include "Weapon.generated.h"

/**
 * 
 */

USTRUCT()
struct FWeaponDataTable: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ClipBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;

};

UCLASS()
class THIRDPERSONESHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;

protected:
	void StopFalling();

	virtual void OnConstruction(const FTransform& WeaponTransform) override;

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	// Amm count for this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=WeaponProperties, meta=(AllowPrivateAccess="true"))
	int32 Ammo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=WeaponProperties, meta=(AllowPrivateAccess="true"))
	int32 MagazineCapacity;

	// type of weapon and ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=WeaponProperties, meta=(AllowPrivateAccess="true"))
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=WeaponProperties, meta=(AllowPrivateAccess="true"))
	EAmmoType AmmoType;

	// name of reload animation section in ReloadAnimationMontage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=WeaponProperties, meta=(AllowPrivateAccess="true"))
	FName ReloadMontageSection;

	// true when moving the clip while reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=WeaponProperties, meta=(AllowPrivateAccess="true"))
	bool bMovingClip;

	// name of gun reload clip bone
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=WeaponProperties, meta=(AllowPrivateAccess="true"))
	FName ClipBoneName;

	// data table for weapon properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	UDataTable* WeaponDataTable;

	int32 PreviousMaterialIndex;
	
	
public:
	// throws the weapon with an impulse
	void ThrowWeapon();
	
	FORCEINLINE int32 GetAmmo() const {return Ammo;};
	FORCEINLINE int32 GetMagazineCapacity() const {return MagazineCapacity;};
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;};
	FORCEINLINE EAmmoType GetAmmoType() const {return AmmoType;};
	
	FORCEINLINE FName GetReloadMontageSection() const {return ReloadMontageSection;};
	FORCEINLINE void SetReloadMontageSection(FName x) {ReloadMontageSection = x;};
	
	FORCEINLINE FName GetClipBoneName() const {return ClipBoneName;};
	FORCEINLINE void SetClipBoneName(FName x) {ClipBoneName = x;};
	
	FORCEINLINE void SetMovingClip(bool Move){bMovingClip = Move;};

	// called from character class when firing weapon
	void DecrementAmmoCount();
	void ReloadAmmo(int32 AMount);
	bool ClipIsFull();
	
};
