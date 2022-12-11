// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// #include "Particles/ParticleSystem.h"
// #include "Sound/SoundCue.h"

AWeapon::AWeapon():

	ThrowWeaponTime(0.7f),
	bFalling(false),
	Ammo(30),
	MagazineCapacity(30),
	WeaponType(EWeaponType::EWT_SMG),
	AmmoType(EAmmoType::EAT_9MM),
	ReloadMontageSection(FName(TEXT("Reload SMG"))),
	ClipBoneName(FName(TEXT("smg_clip"))),
	SlideDisplacement(0.f),
	SlideDisplacementTime(0.2f),
	bMovingSlide(false),
	MaxSlideDisplacement(4.f),
	MaxRecoilRotation(5.f),
	bAutomatic(true)

{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (BoneToHide != FName(""))
	{
		GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep the weapon upright
	// if(GetItemState() == EItemState::EIS_Falling && bFalling)
	// {
	// 	const FRotator MeshRotation{0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
	// 	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	// }

	// update slide on Pistol
	UpdateSlideDisplacement(DeltaTime);
}

void AWeapon::ThrowWeapon()
{
	const FRotator MeshRotation{0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward{GetItemMesh()->GetForwardVector()};
	const FVector MeshRight{GetItemMesh()->GetRightVector()};

	// direction in which we throw the weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);
	const float RandomRotation{FMath::FRandRange(10.f, 30.f)};
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 10000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
	EnableGlowMaterial();
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_Pickup);
	StartPulseTimer();
}

void AWeapon::OnConstruction(const FTransform& WeaponTransform)
{
	Super::OnConstruction(WeaponTransform);
	const FString WeaponTablePath{TEXT("DataTable'/Game/_Game/DataTables/WeaponDataTable.WeaponDataTable'")};

	UDataTable* WeaponTableObject = Cast<UDataTable>(
		StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDataRow{nullptr};
		switch (WeaponType)
		{
		case EWeaponType::EWT_SMG:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SubMachineGun"), TEXT(""));
			break;

		case EWeaponType::EWT_AR:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("AssaultRifle"), TEXT(""));
			break;

		case EWeaponType::EWT_PISTOL:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));
			break;

		default:
			break;
		}

		if (WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagazineCapacity = WeaponDataRow->MagazineCapacity;
			SetPickupSound(WeaponDataRow->PickupSound);
			SetEquipSound(WeaponDataRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);
			SetIconItem(WeaponDataRow->InventoryIcon);

			SetMaterialInstance(WeaponDataRow->MaterialInstance);
			PreviousMaterialIndex = GetMaterialIndex();
			GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);
			SetMaterialIndex(WeaponDataRow->MaterialIndex);

			SetClipBoneName(WeaponDataRow->ClipBoneName);
			SetReloadMontageSection(WeaponDataRow->ReloadMontageSection);

			GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimBP);

			CrosshairsMiddle = WeaponDataRow->CrosshairsMiddle;
			CrosshairsLeft = WeaponDataRow->CrosshairsLeft;
			CrosshairsRight = WeaponDataRow->CrosshairsRight;
			CrosshairsTop = WeaponDataRow->CrosshairsTop;
			CrosshairsBottom = WeaponDataRow->CrosshairsBottom;

			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
			FireSound = WeaponDataRow->FireSound;

			BoneToHide = WeaponDataRow->BoneToHide;
			
			bAutomatic = WeaponDataRow->bAutomatic;
		}

		// update the material which glows on this weapon
		if (GetMaterialInstance())
		{
			SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColor"), GetGlowColor());
			GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());

			EnableGlowMaterial();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponDataRow is empty"));
		}
	}
}

void AWeapon::DecrementAmmoCount()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("Attempted to reload with more than magazine capacity"));
	Ammo += Amount;
}

bool AWeapon::ClipIsFull()
{
	return Ammo >= MagazineCapacity;
}

void AWeapon::StartSlideTimer()
{
	GetWorldTimerManager().SetTimer(
		SlideTimer,
		this,
		&AWeapon::FinishMovingSlide,
		SlideDisplacementTime);
	bMovingSlide = true;
}

void AWeapon::FinishMovingSlide()
{
	bMovingSlide = false;
}

void AWeapon::UpdateSlideDisplacement(float DeltaTime)
{
	if (!bMovingSlide) return;
	if (SlideDisplacementCurve)
	{
		const float ElapsedTime{GetWorldTimerManager().GetTimerElapsed(SlideTimer)};
		const float CurveValue{SlideDisplacementCurve->GetFloatValue(ElapsedTime)};
		SlideDisplacement = CurveValue * MaxSlideDisplacement;
		RecoilRotation = CurveValue * MaxRecoilRotation;
		UE_LOG(LogTemp, Warning, TEXT("RecoilRotation: %f"), RecoilRotation);
	}
}
