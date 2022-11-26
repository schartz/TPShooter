// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UShooterAnimInstance::UShooterAnimInstance():
Speed(0.f),
bIsInAir(false),
bIsAccelrating(false),
MovementOffsetYaw(0.f),
LastMovementOffsetYaw(0.f),
bAiming(false),
CharacterYaw(0.f),
CharacterYawLastFrame(0.f),
RootBoneYawOffset(0.f),
RotationCurve(0.f),
RotationCurveLastFrame(0.f)
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

		/*
		FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), AimRotation.Yaw);
		FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);
		
		FString OffsetYawMessage = FString::Printf(TEXT("Offset Yaw: %f"), MovementOffsetYaw);
		if (GEngine)
		{
		    GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green, OffsetYawMessage);
		}*/
	}

	TurnInPlace();
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;
	if (Speed > 0)
	{
		// Don't want to turn in place; Character is moving
		RootBoneYawOffset = 0.f;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		CharacterYawLastFrame = CharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float YawDelta{ CharacterYaw - CharacterYawLastFrame };

		// Root Yaw Offset, updated and clamped to [-180, 180]
		RootBoneYawOffset = UKismetMathLibrary::NormalizeAxis(RootBoneYawOffset - YawDelta);

		// 1.0 if turning, 0.0 if not
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			// RootBoneYawOffset > 0, -> Turning Left. RootBoneYawOffset < 0, -> Turning Right.
			RootBoneYawOffset > 0 ? RootBoneYawOffset -= DeltaRotation : RootBoneYawOffset += DeltaRotation;

			const float ABSRootBoneYawOffset{ FMath::Abs(RootBoneYawOffset) };
			
			if (ABSRootBoneYawOffset > 90.f)
			{
				const float YawExcess{ ABSRootBoneYawOffset - 90.f };
				RootBoneYawOffset > 0 ? RootBoneYawOffset -= YawExcess : RootBoneYawOffset += YawExcess;
			}
		}

		if (GEngine) GEngine->AddOnScreenDebugMessage(1, -1, FColor::Cyan, FString::Printf(TEXT("RootBoneYawOffset: %f"), RootBoneYawOffset));
	}
}
