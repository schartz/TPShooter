// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "Weapon.h"
#include "WeaponType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UShooterAnimInstance::UShooterAnimInstance():
	Speed(0.f),
	bIsInAir(false),
	bIsAccelrating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	TIPCharacterYaw(0.f),
	TIPCharacterYawLastFrame(0.f),
	RootBoneYawOffset(0.f),
	RotationCurve(0.f),
	RotationCurveLastFrame(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_HIP),
	CharacterRotation(FRotator(0.f)),
	CharacterRotationLastFrame(FRotator(0.f)),
	YawDelta(0.f),
	bCrouching(false),
	WeaponRecoilWeight(1.f),
	bTurningInPlace(false),
	EquippedWeaponType(EWeaponType::EWT_MAX),
	bShouldUseFABRIK(false)
{
}


void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}


void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_RELOADING;
		bEquipping = ShooterCharacter->GetCombatState() == ECombatState::ECS_Equipping;
		bCrouching = ShooterCharacter->GetCrouching();
		bShouldUseFABRIK = (ShooterCharacter->GetCombatState() == ECombatState::ECS_UNOCCUPIED ||
			ShooterCharacter->GetCombatState() == ECombatState::ECS_FIRE_TIMER_IN_PROGRESS);

		// get leteral speed of the character from its velocity
		FVector velocity = ShooterCharacter->GetVelocity();
		velocity.Z = 0;
		Speed = velocity.Size();

		// is the character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// is the character accelerating?
		// here we will use the oversimplification that acceleration = movement
		// I know that is wrong, but is works good enough for our purposes without complicating things
		bIsAccelrating = (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0);

		const FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}


		// is the character aiming?
		bAiming = ShooterCharacter->GetAiming();

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_RELOADING;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_IN_AIR;
		}
		else if (ShooterCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_AIMING;
		}
		else
		{
			OffsetState = EOffsetState::EOS_HIP;
		}

		// Check if the SHooterCharacter has a valid EquippedWeapon
		if (ShooterCharacter->GetEquippedWeapon())
		{
			EquippedWeaponType = ShooterCharacter->GetEquippedWeapon()->GetWeaponType();
		}
	}

	TurnInPlace();
	LeanInDirection(DeltaTime);
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	// set the pitch
	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0 || bIsInAir)
	{
		// Don't want to turn in place; Character is moving
		RootBoneYawOffset = 0.f;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float TIPYawDelta{TIPCharacterYaw - TIPCharacterYawLastFrame};

		// Root Yaw Offset, updated and clamped to [-180, 180]
		RootBoneYawOffset = UKismetMathLibrary::NormalizeAxis(RootBoneYawOffset - TIPYawDelta);

		// 1.0 if turning, 0.0 if not
		const float Turning{GetCurveValue(TEXT("Turning"))};
		if (Turning > 0)
		{
			bTurningInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{RotationCurve - RotationCurveLastFrame};

			// RootBoneYawOffset > 0, -> Turning Left. RootBoneYawOffset < 0, -> Turning Right.
			RootBoneYawOffset > 0 ? RootBoneYawOffset -= DeltaRotation : RootBoneYawOffset += DeltaRotation;

			const float ABSRootBoneYawOffset{FMath::Abs(RootBoneYawOffset)};

			if (ABSRootBoneYawOffset > 90.f)
			{
				const float YawExcess{ABSRootBoneYawOffset - 90.f};
				RootBoneYawOffset > 0 ? RootBoneYawOffset -= YawExcess : RootBoneYawOffset += YawExcess;
			}
		}
		else
		{
			bTurningInPlace = false;
		}


		//if (GEngine) GEngine->AddOnScreenDebugMessage(1, -1, FColor::Cyan, FString::Printf(TEXT("RootBoneYawOffset: %f"), RootBoneYawOffset));
	}

	if (bTurningInPlace)
	{
		if (bReloading || bEquipping)
		{
			WeaponRecoilWeight = 1.f;
		}
		else
		{
			WeaponRecoilWeight = 0.f;
		}
	}
	else // not turning in place
	{
		if (bCrouching)
		{
			if (bReloading || bEquipping)
			{
				WeaponRecoilWeight = 1.f;
			}
			else
			{
				WeaponRecoilWeight = 0.1f;
			}
		}
		else
		{
			if (bAiming || bReloading || bEquipping)
			{
				WeaponRecoilWeight = 1.f;
			}
			else
			{
				WeaponRecoilWeight = 0.5f;
			}
		}
	}
}

void UShooterAnimInstance::LeanInDirection(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	const FRotator Delta{UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame)};

	const float Target{static_cast<float>(Delta.Yaw) / DeltaTime};
	const float Interp{FMath::FInterpTo(YawDelta, Target, DeltaTime, 0.6f)};
	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);

	//if (GEngine) GEngine->AddOnScreenDebugMessage(2, -1, FColor::Magenta, FString::Printf(TEXT("YawDelta: %f"), YawDelta));
	//if (GEngine) GEngine->AddOnScreenDebugMessage(3, -1, FColor::Red, FString::Printf(TEXT("Delta.Yaw: %f"), Delta.Yaw));
}
