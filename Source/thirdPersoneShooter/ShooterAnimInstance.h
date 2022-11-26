// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONESHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();
	
protected:
	// handle turning in place variables
	void TurnInPlace();
	
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

	// yaw of the character this frame;
	float CharacterYaw;

	// yaw of the character previous frame
	float CharacterYawLastFrame;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=TurnInPlace, meta=(AllowPrivateAccess = "true"))
	float RootBoneYawOffset;

	// rotation curve value this frame
	float RotationCurve;

	// rotation curve value last frame
	float RotationCurveLastFrame;

public:

	// this function is like beginPlay but for animations
	virtual void NativeInitializeAnimation() override;

	// this function will act like tick function
	// called every frame
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	
};
