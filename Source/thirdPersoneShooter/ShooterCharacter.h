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

	// Set bAiming to true or false with button press
	void AimingButtonPressed();
	void AimingButtonReleased();

	// update field of view based on aiming
	void UpdateCameraFieldOfView(float DeltaTime);

	// update lookup and turn rates based on aiming
	void UpdateTurnAndLookupRates();
	
	void CalculateCrosshairSpread(float DeltaTime);

	// start a small timer when bullet fires
	void StartCrosshairBulletFire();
	UFUNCTION()
	void FinishCrossHairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();
	void StartFireTimer();
	UFUNCTION()
	void AutoFireReset();
	

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

	// turn rate while not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	float HipTurnRate;

	// look up rate while not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	float HipLookupRate;
	
	// turn rate while aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	float AimingTurnRate;

	// look up rate while aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	float AimingLookupRate;


	
	// Scale factor for mouse X sensitivity. Turn rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"),
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float MouseHipTurnRate;

	// Scale factor for mouse Y sensitivity. Look up rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"),
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float MouseHipLookupRate;
	
	// Scale factor for mouse X sensitivity. Turn rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"),
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float MouseAimingTurnRate;

	// Scale factor for mouse Y sensitivity. Look up rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"),
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float MouseAimingLookupRate;



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

	// determines the spreading of crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Crosshairs, meta=(AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	// velocity component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Crosshairs, meta=(AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	// jumping/falling component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Crosshairs, meta=(AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	// aiming component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Crosshairs, meta=(AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	// firing a weapon component for crosshair spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Crosshairs, meta=(AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	// timers for single bullet fire
	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	// LMB or RCT pressed
	bool bFireButtonPressed;

	// true when we can fire, false when waiting for the timer
	bool bShouldFire;

	// bullet fire rate from a Gun
	float AutoFireRate;

	// sets a timer between two timers
	FTimerHandle AutoFireTimer;


public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const {return CameraBoom;}
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}
	FORCEINLINE bool GetAiming() const {return bAiming;}

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	/**
	 * Called via input to turn at a given rate
	 * @param Rate normalized rate. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	 * Called via input to look up and down at a given rate
	 * @param Rate normalized rate. 1.0 means 100% of desired lookup rate
	*/
	void LookupAtRate(float Rate);

	/**
	 * Rotate controller based on mouse X movement
	 * @param Value normalized value. Input value from mouse movement
	*/
	void TurnByMouse(float Value);

	/**
	 * Rotate controller based on mouse Y movement
	 * @param Value normalized value. Input value from mouse movement
	*/
	void LookupByMouse(float Value);

	// called when the fire button is pressed
	void FireWeapon();

};
