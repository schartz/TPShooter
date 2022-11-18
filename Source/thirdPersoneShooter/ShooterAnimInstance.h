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

public:

	// this function is like beginPlay but for animations
	virtual void NativeInitializeAnimation() override;

	// this function will act like tick function
	// called every frame
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float deltaTime);

	
};
