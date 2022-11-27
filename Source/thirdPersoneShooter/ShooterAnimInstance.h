// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterCharacter.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EOffsetState: uint8
{
	EOS_AIMING UMETA(DisplayName = "Aiming"),
	EOS_HIP UMETA(DisplayName = "Hip"),
	EOS_RELOADING UMETA(DisplayName = "Reloading"),
	EOS_IN_AIR UMETA(DisplayName = "InAIr"),
	
	EOS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class THIRDPERSONESHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();
	
protected:
	// handle turning in place variables
	void TurnInPlace();

	// handle calculations for leaning while running
	void LeanInDirection(float DeltaTime);
	
private:

	// reference to the Character who is using this animation instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	class AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	bool bIsAccelrating;

	// offset yaw used from strafing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	float MovementOffsetYaw;

	// offset yaw of the frame before we stopped moving
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	float LastMovementOffsetYaw;

	// true if our character is aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	bool bAiming;

	// yaw of the character this frame, only used when character is still and not in air;
	float TIPCharacterYaw;

	// yaw of the character previous frame, only used when character is still and not in air;
	float TIPCharacterYawLastFrame;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=TurnInPlace, meta=(AllowPrivateAccess = "true"))
	float RootBoneYawOffset;

	// rotation curve value this frame
	float RotationCurve;

	// rotation curve value last frame
	float RotationCurveLastFrame;

	// the pitch of the aim rotation used for aim offset
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=TurnInPlace, meta=(AllowPrivateAccess = "true"))
	float Pitch;

	// true when reloading, used to stop aim offset while reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=TurnInPlace, meta=(AllowPrivateAccess = "true"))
	bool bReloading;

	// offset state used to determine which offset state to use
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=TurnInPlace, meta=(AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	// yaw of the character this frame, this is for character leaning while running
	FRotator CharacterRotation;

	// yaw of the character previous frame, this is for character leaning while running
	FRotator CharacterRotationLastFrame;

	// Yaw delta used for leaning in the running blendspace
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Lean, meta=(AllowPrivateAccess = "true"))
	float YawDelta;

	// true when crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Crouching, meta=(AllowPrivateAccess = "true"))
	bool bCrouching;

	// change the recoil weight based on turn in place and aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess = "true"))
	float WeaponRecoilWeight;

	// true when turning in place
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess = "true"))
	bool bTurningInPlace;
	

public:

	// this function is like beginPlay but for animations
	virtual void NativeInitializeAnimation() override;

	// this function will act like tick function
	// called every frame
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	
};
