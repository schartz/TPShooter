// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class THIRDPERSONESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// called for forwards/backwards input
	void MoveForward(float value);

	// called for left/right input
	void MoveRight(float value);
	
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	// Set bAiming to tru or false with button press
	void AimingButtonPressed();
	void AimingButtonReleased();
	void UpdateCameraFieldOfView(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



private:
	// base turn rate in degrees/second. other scaling will be applied in further usage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	float BaseTurnRate;

	// base look up / look down rate in degrees/second. other scaling will be applied in further usage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	float BaseLookupRate;



	// camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// a camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// flash on gun barrel upon fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	// particle effect on where bullets hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	// particle effect on the path of bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// randomized weapon fire sound cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	// montage for firing the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// true when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess = "true"))
	bool bAiming;

	// default camera field of view value
	// higher FOV more you see!
	float CameraDefaultFOV;

	// camera field of view value when zoomed in
	float CameraZoomedFOV;

	// camera field of view this frame
	float CameraCurrentFOV;

	// interpolation speed for zooming when aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;


public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const {return CameraBoom;}
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}
	FORCEINLINE bool GetAiming() const {return bAiming;}

	/**
	 * Called via input to turn at a given rate
	 * @param Rate normalized rate. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float rate);

	/**
	 * Called via input to look up and down at a given rate
	 * @param Rate normalized rate. 1.0 means 100% of desired lookup rate
	*/
	void LookupAtRate(float rate);

	// called when the fire button is pressed
	void FireWeapon();

};
