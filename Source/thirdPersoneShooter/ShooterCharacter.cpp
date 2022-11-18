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
	BaseTurnRate(45.f),
	BaseLookupRate(45.f)
	
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create a camera boom
	// it should pull in towards the character if there is a collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

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
	FString myString {TEXT("My Sring!!!")};
	UE_LOG(LogTemp, Warning, TEXT("my FString: %s"), *myString)
	UE_LOG(LogTemp, Warning, TEXT("this instance is: %s"), *GetName());
	
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	PlayerInputComponent->BindAxis("TurnByMouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookupByMouse", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);

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

