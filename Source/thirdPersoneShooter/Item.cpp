// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AItem::AItem():
	ItemName(FString("Item Name Here")),
	ItemCount(1),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_Pickup),

	// item interp variables
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ZCurveTime(0.7f),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f),
	ItemType(EItemType::EIT_Max),
	InterToLocationIndex(0),

	MaterialIndex(0),
	bCanChangeCustomDepth(true),

	// dynamic material parameters
	PulseCurveTime(5.f),
	GlowAmount(150.f),
	FresnelExponent(3.f),
	FresnelReflectFraction(4.f),
	SlotIndex(0),
	bCharacterInventoryFull(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// hide pickup widget
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	// sets active stars based on item rarity
	SetActiveStarts();

	// setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// Set item properties based on it's current state
	SetItemProperties(ItemState);

	// set custom depth to disable
	InitializeCustomDepth();

	// start item flashing
	StartPulseTimer();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// handle the item interping when in the Interping state
	ItemInterp(DeltaTime);

	// Get curve values from Pulse curve and set dynamic material parameters
	UpdatePulse();
}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(ItemState);
}

void AItem::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
			ShooterCharacter->UnHighlightInventorySLot();
		}
	}
}

void AItem::SetActiveStarts()
{
	// we will not use zero element
	// we will treat this array as 1 indexed instead of 0 indexed
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;

	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;

	case EItemRarity::EIR_Uncommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;

	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;

	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;

	default:
		break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EIS_PickedUp:
		PickupWidget->SetVisibility(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Falling:
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);


		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	default:
		break;
	}
}

void AItem::StartItemCurve(AShooterCharacter* InteractingCharacter, bool ForcePlaySound)
{
	// store the handle to the character
	Character = InteractingCharacter;

	// get array index from InterpLocations with the lowest item count
	InterToLocationIndex = Character->GetInterpLocationIndex();
	// Add 1 to the item count for this interp location struct
	Character->IncrementInterpLocationItemCount(InterToLocationIndex, 1);

	// play the item pickup sound
	PlayPickupSound(ForcePlaySound);

	// interp start location is Item's current location
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);
	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::FinishInterping, ZCurveTime);
	GetWorldTimerManager().ClearTimer(PulseTimer);

	// Get initial Yaws of the camera and the item
	const double CameraRotationYaw{Character->GetFollowCamera()->GetComponentRotation().Yaw};
	const double ItemRotationYaw{GetActorRotation().Yaw};

	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

	bCanChangeCustomDepth = false;
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character)
	{
		// subtract 1 from the item count of the interp location struct
		Character->IncrementInterpLocationItemCount(InterToLocationIndex, -1);
		Character->GetPickupItem(this);
		Character->UnHighlightInventorySLot();
	}

	bCanChangeCustomDepth = true;
	DisableGlowMaterial();
	DisableCustomDepth();
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping)
	{
		return;
	}

	if (Character && ItemZCurve)
	{
		// time elapsed since we started our interp timer
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);

		// get curve values corresponding to elapsed time
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);

		// get the items initial location when curve started
		FVector ItemLocation = ItemInterpStartLocation;

		// get location in front of the camera
		const FVector CameraInterpLocation{GetInterpLocation()};

		// vector from item to camera location
		const FVector ItemToCamera{FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z)};

		// scale factor to multiply with the curve value
		const float DeltaZ = ItemToCamera.Size();

		// adding curve value to the z component of the item location, scaled by DeltaZ
		ItemLocation.Z += CurveValue * DeltaZ;


		const FVector CurrentLocation{GetActorLocation()};
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.f);
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.f);

		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		// adjust the item rotation per frame such that it does not rotate WRT camera
		const FRotator CameraRotation{Character->GetFollowCamera()->GetComponentRotation()};
		const FRotator ItemRotation{0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f};
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);
	}
}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr) return FVector(0.f);
	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		return Character->GetInterpLocation(InterToLocationIndex).SceneComponent->GetComponentLocation();
		break;

	case EItemType::EIT_Weapon:
		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
		break;

	default:
		return FVector(0.f);
		break;
	}
}

void AItem::PlayPickupSound(bool bForcePlaySound)
{
	if (Character)
	{
		if (bForcePlaySound)
		{
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
		else if (Character->ShouldPlayPickupSound())
		{
			Character->StartPickupSoundTimer();
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
	if (Character)
	{
		if (bForcePlaySound)
		{
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
		else if (Character->ShouldPlayEquipSound())
		{
			Character->StartEquipSoundTimer();
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}


void AItem::EnableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(true);
	}
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(false);
	}
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	//Super::OnConstruction(Transform);

	/* load data in the item rarity datatable*/
	// 1 path to the item rarity datatable
	auto RarityTablePath = TEXT("DataTable'/Game/_Game/DataTables/ItemRarityDataTable.ItemRarityDataTable'");

	UDataTable* RarityTableObject = Cast<UDataTable>(
		StaticLoadObject(UDataTable::StaticClass(), nullptr, RarityTablePath));
	
	if (RarityTableObject)
	{
		FItemRarityTable* RarityRow = nullptr;
		switch (ItemRarity)
		{
		case EItemRarity::EIR_Damaged:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Damaged"), TEXT(""));
			break;

		case EItemRarity::EIR_Common:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));
			break;

		case EItemRarity::EIR_Uncommon:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Uncommon"), TEXT(""));
			break;

		case EItemRarity::EIR_Rare:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));
			break;

		case EItemRarity::EIR_Legendary:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Legendary"), TEXT(""));
			break;

		default:
			break;
		}

		// check if rarity row is valid
		if(RarityRow)
		{
			GlowColor = RarityRow->GlowColor;
			LightColor = RarityRow->LightColor;
			DarkColor = RarityRow->DarkColor;
			NumberOfStars = RarityRow->NumberOfStars;
			IconBackGround = RarityRow->IconBackground;
			if(GetItemMesh())
			{
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
			}
		}
	}

	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("FresnelColor"), GlowColor);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);

		EnableGlowMaterial();
	}
}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
	}
}


void AItem::StartPulseTimer()
{
	if (ItemState == EItemState::EIS_Pickup)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::UpdatePulse()
{
	FVector CurveValue{};
	switch (ItemState)
	{
	case EItemState::EIS_Pickup:
		if (PulseCurve)
		{
			float ELapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
			CurveValue = PulseCurve->GetVectorValue(ELapsedTime);
		}
		break;

	case EItemState::EIS_EquipInterping:
		if (InterpPulseCurve)
		{
			float ELapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
			CurveValue = InterpPulseCurve->GetVectorValue(ELapsedTime);
		}
		break;
	};

	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X * GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), CurveValue.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(
			TEXT("FresnelReflectFraction"), CurveValue.Z * FresnelReflectFraction);
	}
}
