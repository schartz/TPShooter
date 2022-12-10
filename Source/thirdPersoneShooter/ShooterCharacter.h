// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombatState: uint8
{
	ECS_UNOCCUPIED UMETA(DisplayName = "Unoccupied"),	
	ECS_FIRE_TIMER_IN_PROGRESS UMETA(DisplayName = "FireTimerInProgress"),	
	ECS_RELOADING UMETA(DisplayName = "Reloading"),	
	ECS_Equipping UMETA(DisplayName = "Equipping"),	
	ECS_MAX UMETA(DisplayName = "DisplayMax"),	
};

USTRUCT(BlueprintType)
struct  FInterpLocation
{
	GENERATED_BODY()

	// USceneComponent to be used for its location as interping finish location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	// number of items interping to this location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSLotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

UCLASS()
class THIRDPERSONESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	inline static const float MAX_TRACE_DISTANCE_FOR_SHOOTING{50000.f};
	inline static const float MAX_TRACE_DISTANCE_FOR_ITEMS {20000.f};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// called for forwards/backwards input
	void MoveForward(float value);

	// called for left/right input
	void MoveRight(float value);
	
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation) const;

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

	void TakeActionButtonPressed();
	void TakeActionButtonReleased();

	UFUNCTION()
	void AutoFireReset();

	// line trace for items under the crosshair
	bool TraceUnderCrosshair(FHitResult& OutHitResult, FVector& OutHitLocation) const;

	// Trace for items in visibity is overlapped item count > 0
	void TraceForItems();

	// Spawns a default weapon and equips it
	class AWeapon* SpawnDefaultWeapon() const;

	// take a weapon and equips it
	void EquipWeapon(class AWeapon* WeaponToEquip, bool bSwapping = false);

	// drop the currently Equipped weapon
	void DropWeapon();

	// Drop currently equipped weapon and equip WeaponToSwap
	void SwapWeapon(class AWeapon* WeaponToSwap);

	// initialize the AmmoMap with Ammo values
	void InitializeAmmoMap();

	// checks is weapon has ammo
	bool WeaponHasAmmo() const;

	// fire weapon functions
	void PlayFireSound() const;
	void SendBullet() const;
	void PlayFireWeaponAnimation() const;

	// reload weapon functions
	void ReloadButtonPressed();
	void ReloadWeapon();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	// weapon swap and equip function
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	// checks to see if we have ammo for the equipped weapon type
	bool CarryingAmmo();

	// called from animation blueprint with grabClip notifier
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	// called from animation blueprint with releaseClip notifier
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void CrouchButtonPressed();

	// function to handle jump button pressed
	virtual void Jump() override;

	// interps capsule half height when crouching/standing
	void InterpCapsuleHalfHeight(float DeltaTime);

	void StartAiming();
	void StopAiming();

	void PickupAmmo(class AAmmo* Ammo);

	void InitializeInterpLocations();

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();
	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);
	int32 GetEmptyInventorySLot();
	void HighlightInventorySLot();

	

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

	// particle effect on where bullets hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	// particle effect on the path of bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// montage for firing the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// true when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess = "true"))
	bool bAiming;

	// default camera field of view value
	// higher FOV more you see!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera, meta=(AllowPrivateAccess = "true"))
	float CameraDefaultFOV;

	// camera field of view value when zoomed in
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera, meta=(AllowPrivateAccess = "true"))
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

	// sets a timer between two timers
	FTimerHandle AutoFireTimer;

	// true if we should trace for items every frame
	bool bShouldTraceForItems;

	// number of currently overlapping items
	int8 OverlappedItemCount;

	// The item currently hit by our trace in TraceForItems (could be null)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Items, meta=(AllowPrivateAccess="true"))
	class AItem* TracedHitItem;

	// keeps track of the last item traced
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Items, meta=(AllowPrivateAccess="true"))
	class AItem* TracedHitItemLastFrame;

	// currently equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	class AWeapon* EquippedWeapon;

	// Set this in blueprints for the defaults weapons class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// distance outward from the camera for the interp destination
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Items, meta=(AllowPrivateAccess="true"))
	float CameraInterpDistance;


	// distance upward from the camera for the interp destination
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Items, meta=(AllowPrivateAccess="true"))
	float CameraInterpElevation;
	
	// map to keep track of ammo of the different ammo types
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Items, meta=(AllowPrivateAccess="true"))
	TMap<EAmmoType, int32> AmmoMap;

	// starting amount of 9mm
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Items, meta=(AllowPrivateAccess="true"))
	int32 Starting9MMAmmo;

	// starting amount of AR ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Items, meta=(AllowPrivateAccess="true"))
	int32 StartingARAmmo;

	// combat state can only fire and reload if unoccupied
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	ECombatState CombatState;

	// montage for reload animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess="true"))
	class UAnimMontage* ReloadMontage;

	// montage for equip weapon animations while swapping weapons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess="true"))
	class UAnimMontage* EquipMontage;

	// transform of the clip when we first grab the clip during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	FTransform CLipTransform;

	// scene component to be attached to the character's hand during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	class USceneComponent* HandSceneComponent;

	// true when crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	bool bCrouching;

	// normal movement speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	float BaseMovementSpeed;

	// movement speed while crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	float CrouchMovementSpeed;

	// half heights of the collision capsule half heights
	float CurrentCapsuleHalfHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(AllowPrivateAccess="true"))
	float StandingCapsuleHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(AllowPrivateAccess="true"))
	float CrouchingCapsuleHalfHeight;

	
	// friction variables for crouching and standing states
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(AllowPrivateAccess="true"))
	float BaseGroundFriction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta=(AllowPrivateAccess="true"))
	float CrouchingGroundFriction;

	// true when aiming button is pressed
	bool bAimingButtonPressed;


	// extra USceneComponent to be used for various interpolation end points
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SceneComponents, meta=(AllowPrivateAccess="true"))
	class USceneComponent* WeaponInterpComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SceneComponents, meta=(AllowPrivateAccess="true"))
	class USceneComponent* InterpComp1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SceneComponents, meta=(AllowPrivateAccess="true"))
	class USceneComponent* InterpComp2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SceneComponents, meta=(AllowPrivateAccess="true"))
	class USceneComponent* InterpComp3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SceneComponents, meta=(AllowPrivateAccess="true"))
	class USceneComponent* InterpComp4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SceneComponents, meta=(AllowPrivateAccess="true"))
	class USceneComponent* InterpComp5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SceneComponents, meta=(AllowPrivateAccess="true"))
	class USceneComponent* InterpComp6;

	// array of interp location structs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=SceneComponents, meta=(AllowPrivateAccess="true"))
	TArray<FInterpLocation> InterpLocations;
	
	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	// time to wait before we can play another pickup sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemSounds, meta=(AllowPrivateAccess="true"))
	float PickupSoundResetTime;
	// time to wait before we can play another equip sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemSounds, meta=(AllowPrivateAccess="true"))
	float EquipSoundResetTime;

	// an array of AItems for our inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Inventory, meta=(AllowPrivateAccess="true"))
	TArray<class AItem*> Inventory;

	const int32 INVENTORY_CAPACITY{6};


	// delegate for sending slot information to the inventory bar when equipping
	UPROPERTY(BlueprintAssignable, Category=Delegates, meta=(AllowPrivateAccess="true"))
	FEquipItemDelegate EquipItemDelegate;

	// delegate for sending slot information to the weapon slot for playing icon animation
	UPROPERTY(BlueprintAssignable, Category=Delegates, meta=(AllowPrivateAccess="true"))
	FHighlightIconDelegate HighlightIconDelegate;

	// the index for the currently highlighted slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Inventory, meta=(AllowPrivateAccess="true"))
	int32 HighlightedSlot;
	
	

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const {return CameraBoom;}
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}
	FORCEINLINE bool GetAiming() const {return bAiming;}
	FORCEINLINE int8 GetOverlappedItemCount() const {return OverlappedItemCount;}
	FORCEINLINE ECombatState GetCombatState() const {return CombatState;}
	FORCEINLINE bool GetCrouching() const {return bCrouching;}
	FORCEINLINE bool ShouldPlayPickupSound() const {return bShouldPlayPickupSound;}
	FORCEINLINE bool ShouldPlayEquipSound() const {return bShouldPlayEquipSound;}
	FORCEINLINE AWeapon* GetEquippedWeapon() const {return EquippedWeapon;}
	void UnHighlightInventorySLot();

	
	FInterpLocation GetInterpLocation(int32 idx);

	/**
	 * Adds or subtracts to and from OverlappedItemCount.
	 * Also updates the value of bShouldTraceForItems
	 */
	void IncrementOverlappedItemCount(int8 Amount);

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

	// No Longer needed. AItem now has GetInterpLocation method
	//FVector GetCameraInterpLocation() const;

	void GetPickupItem(class AItem* Item);

	// returns an index from the InterpLocations array with the lowest item count
	int32 GetInterpLocationIndex();

	void IncrementInterpLocationItemCount(int32 Index, int32 Amount);

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

};
