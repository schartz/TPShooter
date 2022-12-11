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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CrosshairsMiddle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* FireSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic;
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
	virtual void BeginPlay() override;
	void FinishMovingSlide();
	void UpdateSlideDisplacement(float DeltaTime);

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

	/** Textures for the weapon crosshairs*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	UTexture2D* CrosshairsMiddle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	UTexture2D* CrosshairsRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	UTexture2D* CrosshairsLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	UTexture2D* CrosshairsBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	UTexture2D* CrosshairsTop;

	// the speed at which automatic fire happens
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	float AutoFireRate;

	// particle system spawned at the barrel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	UParticleSystem* MuzzleFlash;

	// sound played when the weapon is fired
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	USoundCue* FireSound;

	// name of the bone to hide on the weapon mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=DataTable, meta=(AllowPrivateAccess="true"))
	FName BoneToHide;

	// amount that the slide is pushed back during pistol fire
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Pistol, meta=(AllowPrivateAccess="true"))
	float SlideDisplacement;

	// curve for the slide displacement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pistol, meta=(AllowPrivateAccess="true"))
	class UCurveFloat* SlideDisplacementCurve;

	// timer handle for updating slide displacement
	FTimerHandle SlideTimer;

	// time for displacing the slide during pistol fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pistol, meta=(AllowPrivateAccess="true"))
	float SlideDisplacementTime;

	// true when Pistol slide is moving
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Pistol, meta=(AllowPrivateAccess="true"))
	bool bMovingSlide;

	// max distance for the slide movement on pistol
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pistol, meta=(AllowPrivateAccess="true"))
	float MaxSlideDisplacement;

	// max rotation for pistol rotate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pistol, meta=(AllowPrivateAccess="true"))
	float MaxRecoilRotation;

	// amount that the pistol will rotate during pistol fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pistol, meta=(AllowPrivateAccess="true"))
	float RecoilRotation;

	// true if weapon is of automatic fire type
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=WeaponProperties, meta=(AllowPrivateAccess="true"))
	bool bAutomatic;
	
	
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

	FORCEINLINE float GetAutoFireRate() const {return AutoFireRate;};
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const {return MuzzleFlash;};
	FORCEINLINE USoundCue* GetFireSound() const {return FireSound;};
	
	FORCEINLINE bool GetAutomatic() const {return bAutomatic;};

	void StartSlideTimer();

	// called from character class when firing weapon
	void DecrementAmmoCount();
	void ReloadAmmo(int32 AMount);
	bool ClipIsFull();
	
};
