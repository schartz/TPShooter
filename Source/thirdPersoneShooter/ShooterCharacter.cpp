// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Ammo.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Item.h"
#include "SWarningOrErrorBox.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	// turn and look up rates base values
	BaseTurnRate(45.f),
	BaseLookupRate(45.f),

	// turn and look up rates for aiming/notaiming
	HipTurnRate(90.f),
	HipLookupRate(90.f),
	AimingTurnRate(20.f),
	AimingLookupRate(20.f),

	// mouse look sensitivity scale factors
	MouseHipTurnRate(1.f),
	MouseHipLookupRate(1.f),
	MouseAimingTurnRate(0.6f),
	MouseAimingLookupRate(0.6f),

	// true when aiming the weapon
	bAiming(false),

	// camera field of view values
	CameraDefaultFOV(0.f), // we're setting this in `BeginPlay` function
	CameraZoomedFOV(30.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),

	// cross hair spread factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),

	// bullet fire timer variables
	ShootTimeDuration(0.05f),
	bFiringBullet(false),

	// automatic fire variables
	bFireButtonPressed(false),
	bShouldFire(true),

	// automatic gun fire rate
	AutoFireRate(0.1f),

	// item trace variables
	bShouldTraceForItems(false),

	OverlappedItemCount(0),

	// camera interp location variables
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),

	// starting ammo amount
	Starting9MMAmmo(50),
	StartingARAmmo(120),

	// combat variables
	CombatState(ECombatState::ECS_UNOCCUPIED),
	bCrouching(false),

	BaseMovementSpeed(650.f),
	CrouchMovementSpeed(300.f),
	CurrentCapsuleHalfHeight(88.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(100.f),
	bAimingButtonPressed(false),

	// pickup and equip sound timer properties
	bShouldPlayPickupSound(true),
	bShouldPlayEquipSound(true),
	PickupSoundResetTime(0.2f),
	EquipSoundResetTime(0.2f)


{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create a camera boom
	// it should pull in towards the character if there is a collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	// create a camera
	// attach it to the end of spring arm connected to that character
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // camera does not rotate relative to cameraboom

	// character should not rotate with mouse or rstick inpout
	// let the controller only affect the character on yaw
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);


	// configure in air speed and control over movement extent
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// create HandSceneComponent
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	// Construct various USceneComponents
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 1"));
	InterpComp1->SetupAttachment(GetFollowCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 2"));
	InterpComp2->SetupAttachment(GetFollowCamera());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 3"));
	InterpComp3->SetupAttachment(GetFollowCamera());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 4"));
	InterpComp4->SetupAttachment(GetFollowCamera());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 5"));
	InterpComp5->SetupAttachment(GetFollowCamera());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 6"));
	InterpComp6->SetupAttachment(GetFollowCamera());
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// this is Unreal engines internal string type
	// * here is an overloaded method to get C style string from FString object
	// here, it is NOT pointer dereferencing
	FString msg{TEXT("Beginng gameplay!!!")};
	UE_LOG(LogTemp, Warning, TEXT("Init message: %s"), *msg)
	UE_LOG(LogTemp, Warning, TEXT("his instance is: %s"), *GetName());

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	// Spawn the default weapon attach this to the mesh
	EquipWeapon(SpawnDefaultWeapon());

	InitializeAmmoMap();

	// set character walk speed
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	// create FInterpLocation structs for each interp location. Add them all to the InterpLocations array
	InitializeInterpLocations();
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// update camera FOV if aiming button is pressed
	UpdateCameraFieldOfView(DeltaTime);

	// change look sensitivity based on aiming
	UpdateTurnAndLookupRates();

	CalculateCrosshairSpread(DeltaTime);

	// check OverlappedItemCount then trace for items
	TraceForItems();

	// interpolate the capsule half height based on crouching/standing
	InterpCapsuleHalfHeight(DeltaTime);
}

void AShooterCharacter::UpdateCameraFieldOfView(float DeltaTime)
{
	if (bAiming)
	{
		if (CameraCurrentFOV == CameraZoomedFOV)
		{
			return;
		}
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
	else
	{
		if (CameraCurrentFOV == CameraDefaultFOV)
		{
			return;
		}
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookupRate", this, &AShooterCharacter::LookupAtRate);

	PlayerInputComponent->BindAxis("TurnByMouse", this, &AShooterCharacter::TurnByMouse);
	PlayerInputComponent->BindAxis("LookupByMouse", this, &AShooterCharacter::LookupByMouse);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("TakeActionButton", IE_Pressed, this, &AShooterCharacter::TakeActionButtonPressed);
	PlayerInputComponent->BindAction("TakeActionButton", IE_Released, this,
	                                 &AShooterCharacter::TakeActionButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);
}


void AShooterCharacter::MoveForward(float value)
{
	if ((Controller != nullptr) && value != 0.0f)
	{
		// find out which way is forward direction
		// yay is rotation along z axis while you are standing in x-y plane
		// like going round and round standing on one place
		const FRotator rotation{Controller->GetControlRotation()};
		const FRotator yawRotation{0, rotation.Yaw, 0};
		const FVector direction{FRotationMatrix{yawRotation}.GetUnitAxis(EAxis::X)};

		// move into that direction
		AddMovementInput(direction, value);
	}
}

void AShooterCharacter::MoveRight(float value)
{
	if ((Controller != nullptr) && value != 0.0f)
	{
		// find out which way is right direction
		// yay is rotation along z axis while you are standing in x-y plane
		// like going round and round standing on one place
		const FRotator rotation{Controller->GetControlRotation()};
		const FRotator yawRotation{0, rotation.Yaw, 0};
		const FVector direction{FRotationMatrix{yawRotation}.GetUnitAxis(EAxis::Y)};

		// move into that direction
		AddMovementInput(direction, value);
	}
}

void AShooterCharacter::TurnAtRate(float rate)
{
	// when you look left and right before crossing a road, that rotatory motion is called yaw
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookupAtRate(float rate)
{
	// when you up and down, that rotatory motion of your head is called pitch
	AddControllerPitchInput(rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::TurnByMouse(float Value)
{
	float ScaleFactor;
	if (bAiming)
	{
		ScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		ScaleFactor = MouseHipLookupRate;
	}

	AddControllerYawInput(Value * ScaleFactor);
}

void AShooterCharacter::LookupByMouse(float Value)
{
	float ScaleFactor;
	if (bAiming)
	{
		ScaleFactor = MouseAimingLookupRate;
	}
	else
	{
		ScaleFactor = MouseHipLookupRate;
	}

	AddControllerPitchInput(Value * ScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_UNOCCUPIED) return;

	if (WeaponHasAmmo())
	{
		// Play sound of gunfire
		PlayFireSound();

		// send bullet
		SendBullet();

		// Play hip fire montage
		PlayFireWeaponAnimation();

		// decrease ammo count
		EquippedWeapon->DecrementAmmoCount();

		// start bullet fire timer for cross hairs
		StartCrosshairBulletFire();

		// start time between shots
		StartFireTimer();
	}
}

void AShooterCharacter::PlayFireSound() const
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AShooterCharacter::SendBullet() const
{
	// show fire flashes on the barrel
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		// position of the muzzle barrel of the gun
		const FTransform socketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, socketTransform);
		}

		FVector BeamEndPoint;
		bool bBeamEnd = GetBeamEndLocation(socketTransform.GetLocation(), BeamEndPoint);
		if (bBeamEnd)
		{
			// spawn impact particles after updating BeamEndpoint
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEndPoint);
			}

			if (BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(), BeamParticles, socketTransform);

				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
				}
			}
		}
	}
}

void AShooterCharacter::PlayFireWeaponAnimation() const
{
	// trigger fire animation in the character
	// we are using animation montage functionality of UE for this
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance and HipFireMontage)
	{
		animInstance->Montage_Play(HipFireMontage);
		animInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool AShooterCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FVector& OutBeamLocation) const
{
	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshair(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		// Tentative beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else // no crosshair trace hit
	{
		// OutBeamLocation is the End location for the line trace
	}

	// Perform a second trace, this time from the gun barrel
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{MuzzleSocketLocation};
	const FVector StartToEnd{OutBeamLocation - WeaponTraceStart};
	const FVector WeaponTraceEnd{MuzzleSocketLocation + StartToEnd * 1.25f};
	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit) // object between barrel and BeamEndPoint?
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}

	return false;
}

/***************************Utility Functions******************/

void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_RELOADING)
	{
		StartAiming();
	}
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::UpdateTurnAndLookupRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookupRate = AimingLookupRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookupRate = HipLookupRate;
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	// calculate CrosshairVelocityFactor
	FVector2d WalkSpeedRange{0.f, 600.f};
	FVector2d VelocityFactorRange{0.f, 1.f};
	FVector CurrentVelocity{GetVelocity()};
	CurrentVelocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityFactorRange,
		CurrentVelocity.Size());


	// calculate CrosshairInAirFactor 
	if (GetCharacterMovement()->IsFalling())
	{
		// while in air, spread crosshair slowly
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f);
	}
	else
	{
		// while on ground shrink crosshair quickly
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	// calculate CrosshairAimFactor
	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.5f, DeltaTime, 5.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 5.f);
	}

	// calculate CrosshairShootingFactor
	// this is True 0.05 seconds after firing a single bullet
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.4f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor + CrosshairAimFactor +
		CrosshairShootingFactor;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshair(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TracedHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if (TracedHitItem and TracedHitItem->GetPickupWidget())
			{
				TracedHitItem->GetPickupWidget()->SetVisibility(true);
			}

			if (TracedHitItemLastFrame)
			{
				if (TracedHitItem != TracedHitItemLastFrame)
				{
					// we are hitting a different AItem this frame from last frame
					// or AItem us null
					TracedHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}

			// store a reference to HitItem for nex frame
			TracedHitItemLastFrame = TracedHitItem;
		}
	}
	else if (TracedHitItemLastFrame)
	{
		// No longer overlapping with any item
		// TraceHitItemLastFrame should not show widget
		TracedHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&AShooterCharacter::FinishCrossHairBulletFire,
		ShootTimeDuration);
}

void AShooterCharacter::FinishCrossHairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::TakeActionButtonPressed()
{
	if (TracedHitItem)
	{
		// start the item interpolation curve for pickup
		TracedHitItem->StartItemCurve(this);
	}
}

void AShooterCharacter::TakeActionButtonReleased()
{
}

void AShooterCharacter::ReloadButtonPressed()
{
	if (EquippedWeapon == nullptr) return;
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_UNOCCUPIED) return;

	// do we have ammo of the correct type
	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		// stop aiming before reloading the weapon
		if (bAiming)
		{
			StopAiming();
		}

		CombatState = ECombatState::ECS_RELOADING;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (ReloadMontage && AnimInstance)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

void AShooterCharacter::FinishReloading()
{
	CombatState = ECombatState::ECS_UNOCCUPIED;

	// aiming button was pressed while reloading, Now is the time to aim
	if (bAimingButtonPressed)
	{
		StartAiming();
	}

	if (EquippedWeapon == nullptr) return;

	const auto AmmoType = EquippedWeapon->GetAmmoType();
	if (AmmoMap.Contains(AmmoType))
	{
		// amount of ammo which the character is carrying of the equipped weapon type
		int32 CarriedAmmo = AmmoMap[AmmoType];

		// space left in the magazine of equipped weapon
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();
		if (MagEmptySpace > CarriedAmmo)
		{
			// fill the magazine with all the ammo character is carrying
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
		}
		else
		{
			// reload the magazine
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
		}

		AmmoMap.Add(AmmoType, CarriedAmmo);
	}
}

void AShooterCharacter::StartFireTimer()
{
	CombatState = ECombatState::ECS_FIRE_TIMER_IN_PROGRESS;
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutoFireRate);
}


void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_UNOCCUPIED;
	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		// reload weapon
		ReloadWeapon();
	}
}

bool AShooterCharacter::TraceUnderCrosshair(
	FHitResult& OutHitResult,
	FVector& OutHitLocation) const
{
	// Get Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// Get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// Trace from Crosshair world location outward
		const FVector Start{CrosshairWorldPosition};
		const FVector End{Start + CrosshairWorldDirection * AShooterCharacter::MAX_TRACE_DISTANCE_FOR_SHOOTING};
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon() const
{
	// check the TSubClassOf variable
	if (DefaultWeaponClass)
	{
		// spawn the weapon
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}

	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		// Get the socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));

		if (HandSocket)
		{
			// attach the weapon to the HandSocket RightHandSocket
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
			EquippedWeapon = WeaponToEquip;

			// update the item state
			EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
		}
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TracedHitItem = nullptr;
	TracedHitItemLastFrame = nullptr;
}

// no longer needed. AItem now has GetInterpLocation method
/*FVector AShooterCharacter::GetCameraInterpLocation() const
{
	const FVector CameraLocation{FollowCamera->GetComponentLocation()};
	const FVector CameraForward{FollowCamera->GetForwardVector()};

	//Can also do CameraUp = FVector CameraUp{FollowCamera->GetUpVector()}; CameraUp * CameraInterpElevation;
	// instead of FVector(0.f, 0.f, CameraInterpElevation)

	return CameraLocation + CameraForward * CameraInterpDistance + FVector(0.f, 0.f, CameraInterpElevation);
}*/

void AShooterCharacter::GetPickupItem(AItem* Item)
{
	// play item equip sound
	Item->PlayEquipSound();

	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		SwapWeapon(Weapon);
	}

	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9MM, Starting9MMAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool AShooterCharacter::WeaponHasAmmo() const
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;
	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void AShooterCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr || HandSceneComponent == nullptr) return;

	// index for the clip bone on the equipped weapon
	const int32 ClipBoneIndex = EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName());

	// store the transform of the clip
	FTransform ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l")));

	HandSceneComponent->SetWorldTransform(ClipTransform);
	EquippedWeapon->SetMovingClip(true);
}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}

	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		ACharacter::Jump();
	}

	//ACharacter::Jump();		
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	const float HalfHeight{
		FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f)
	};

	// DeltaCapsuleHalfHeight is -ve if crouching, +ve if standing
	const float DeltaCapsuleHalfHeight = HalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// we need to put character mesh UP while couching and DOWN while standing
	const FVector MeshOffset{0.f, 0.f, -DeltaCapsuleHalfHeight};

	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(HalfHeight);
}

void AShooterCharacter::StartAiming()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
	// check if AmmoMap already contains this type of ammo
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		// update the amount of ammo in the AMmoMap
		AmmoMap[Ammo->GetAmmoType()] = AmmoMap[Ammo->GetAmmoType()] + Ammo->GetItemCount();
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	// clean up this ammo object
	Ammo->Destroy();
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{WeaponInterpComp, 0};
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{InterpComp1, 0};
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{InterpComp2, 0};
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{InterpComp3, 0};
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{InterpComp4, 0};
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{InterpComp5, 0};
	InterpLocations.Add(InterpLoc5);

	FInterpLocation InterpLoc6{InterpComp6, 0};
	InterpLocations.Add(InterpLoc6);
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;

	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}

	return LowestIndex;
}

void AShooterCharacter::IncrementInterpLocationItemCount(int32 Index, int32 Amount)
{
	// accept only Amount == 1 and Amount == -1
	if (Amount < -1 || Amount > 1) return;

	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}


FInterpLocation AShooterCharacter::GetInterpLocation(int32 idx)
{
	// .Num is line python len - 1
	if (idx <= InterpLocations.Num())
	{
		return InterpLocations[idx];
	}

	return FInterpLocation();
}


void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer,
		this,
		&AShooterCharacter::ResetPickupSoundTimer,
		PickupSoundResetTime);
	
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer,
		this,
		&AShooterCharacter::ResetEquipSoundTimer,
		EquipSoundResetTime);
}

void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}
