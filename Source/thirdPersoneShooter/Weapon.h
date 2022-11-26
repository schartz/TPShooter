// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Weapon.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	EWT_SMG UMETA(DisplayName = "Sub Machine Gun"),
	EWT_AR UMETA(DisplayName = "Assault Rifle"),
	EWT_MAX UMETA(DisplayName = "DefaultMax"),
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
	
	
public:
	// throws the weapon with an impulse
	void ThrowWeapon();
	
	FORCEINLINE int32 GetAmmo() const {return Ammo;};
	FORCEINLINE int32 GetMagazineCapacity() const {return MagazineCapacity;};
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;};
	FORCEINLINE EAmmoType GetAmmoType() const {return AmmoType;};
	FORCEINLINE FName GetReloadMontageSection() const {return ReloadMontageSection;};
	FORCEINLINE FName GetClipBoneName() const {return ClipBoneName;};
	FORCEINLINE void SetMovingClip(bool Move){bMovingClip = Move;};

	// called from character class when firing weapon
	void DecrementAmmoCount();
	void ReloadAmmo(int32 AMount);
	bool ClipIsFull();
	
};
