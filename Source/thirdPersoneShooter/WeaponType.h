#pragma once

UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	EWT_SMG UMETA(DisplayName = "Sub Machine Gun"),
	EWT_AR UMETA(DisplayName = "Assault Rifle"),
	EWT_MAX UMETA(DisplayName = "DefaultMax"),
};