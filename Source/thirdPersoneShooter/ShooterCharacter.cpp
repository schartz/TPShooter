// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
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
MouseAimingTurnRate(0.5f),
MouseAimingLookupRate(0.5f),

// true when aiming the weapon
bAiming(false),

// camera field of view values
CameraDefaultFOV(0.f), // we're setting this in `BeginPlay` function
CameraZoomedFOV(38.f),
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
AutoFireRate(0.1f)

	
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create a camera boom
	// it should pull in towards the character if there is a collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 80.f);

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


}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// this is Unreal engines internal string type
	// * here is an overloaded method to get C style string from FString object
	// here, it is NOT pointer dereferencing
	FString msg {TEXT("Beginng gameplay!!!")};
	UE_LOG(LogTemp, Warning, TEXT("Init message: %s"), *msg)
	UE_LOG(LogTemp, Warning, TEXT("his instance is: %s"), *GetName());

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	
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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

}



void AShooterCharacter::MoveForward(float value)
{
	if ((Controller != nullptr) && value != 0.0f) {
		// find out which way is forward direction
		// yay is rotation along z axis while you are standing in x-y plane
		// like going round and round standing on one place
		const FRotator rotation{Controller->GetControlRotation()}; 
		const FRotator yawRotation{0, rotation.Yaw, 0}; 
		const FVector direction {FRotationMatrix{yawRotation}.GetUnitAxis(EAxis::X)};

		// move into that direction
		AddMovementInput(direction, value);
	}
}

void AShooterCharacter::MoveRight(float value)
{
	if ((Controller != nullptr) && value != 0.0f) {
		// find out which way is right direction
		// yay is rotation along z axis while you are standing in x-y plane
		// like going round and round standing on one place
		const FRotator rotation{Controller->GetControlRotation()}; 
		const FRotator yawRotation{0, rotation.Yaw, 0}; 
		const FVector direction {FRotationMatrix{yawRotation}.GetUnitAxis(EAxis::Y)};

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
	} else
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
	} else
	{
		ScaleFactor = MouseHipLookupRate;
	}

	AddControllerPitchInput(Value * ScaleFactor);
}

void AShooterCharacter::FireWeapon()
{	
	// Play sound of gunfire
	if(FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}


	// show fire flashes on the barrel
	const USkeletalMeshSocket* barrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if(barrelSocket)
	{	
		// position of the muzzle barrel of the gun
		const FTransform socketTransform = barrelSocket->GetSocketTransform(GetMesh());

		if(MuzzleFlash)
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

			if(BeamParticles)
			{	
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, socketTransform);
				
				if(Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
				}
			}
		}
	}
	// trigger fire animation in the character
	// we are using animation montage functionality of UE for this
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if(animInstance and HipFireMontage)
	{
		animInstance->Montage_Play(HipFireMontage);
		animInstance->Montage_JumpToSection(FName("StartFire"));
	}

	// start bullet fire time for cross hairs
	StartCrosshairBulletFire();
	
}



bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation){
	// get current viewport size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X/2.f, ViewportSize.Y/2.f);
	CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;


	// get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0), 
		CrosshairLocation, 
		CrosshairWorldPosition, 
		CrosshairWorldDirection
	);

	if (bScreenToWorld) // was deprojection successful?
	{	
		float MAX_TRACE_DISTANCE {50'000.f};
		FHitResult ScreenTraceHit;
		const FVector Start{CrosshairWorldPosition};
		const FVector End{CrosshairWorldPosition + CrosshairWorldDirection * MAX_TRACE_DISTANCE};


		// set beam endpoint to line trace endpoint
		OutBeamLocation = End;

		// trace outward from crosshairs world location
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit) // was there a trace hit?
		{	

			// beam endpoint is now trace hit location
			OutBeamLocation = ScreenTraceHit.Location;

			// perform a second trace this time from the gun barrel
			FHitResult WeaponTraceHit;
			const FVector WeaponTraceStart{MuzzleSocketLocation};
			const FVector WeaponTraceEnd{OutBeamLocation};
			GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
			if (WeaponTraceHit.bBlockingHit) // object between gun barrel and BeamEndPoint?
			{
				OutBeamLocation = WeaponTraceHit.Location;
			}
		}

		return true;
	}
	return false;
}

/***************************Utility Functions******************/

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AShooterCharacter::UpdateTurnAndLookupRates()
{
	if(bAiming)
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
	FVector2d WalkSpeedRange {0.f, 600.f};
	FVector2d VelocityFactorRange{0.f, 1.f};
	FVector CurrentVelocity {GetVelocity()};
	CurrentVelocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityFactorRange,
		CurrentVelocity.Size());

	
	// calculate CrosshairInAirFactor 
	if(GetCharacterMovement()->IsFalling())
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
	if(bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.5f, DeltaTime, 5.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 5.f);
	}

	// calculate CrosshairShootingFactor
	// this is True 0.05 seconds after firing a single bullet
	if(bFiringBullet)
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
	StartFireTimer();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutoFireRate);
	}
}

void AShooterCharacter::AutoFireReset()
{
	bShouldFire = true;
	if(bFireButtonPressed)
	{
		StartFireTimer();
	}
}

