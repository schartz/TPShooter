// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

// Enum class for maintaining item rarity
UENUM(BlueprintType)
enum class EItemRarity:uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_Max UMETA(DisplayName = "Default_MAXED")
};

// Enum class for maintaining item states
UENUM(BlueprintType)
enum class EItemState:uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickupedUp UMETA(DisplayName = "PickupedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_Max UMETA(DisplayName = "Default_MAXED")
};

// Enum class for item types
UENUM(BlueprintType)
enum class EItemType:uint8
{
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Ammo UMETA(DisplayName = "Ammo"),

	EIT_Max UMETA(DisplayName = "Default_MAXED")
};

UCLASS()
class THIRDPERSONESHOOTER_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when overlapping area sphere begins
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                          AActor* OtherActor,
	                          UPrimitiveComponent* OtherComponent,
	                          int32 OtherBodyIndex,
	                          bool bFromSweep,
	                          const FHitResult& SweepResult);

	// Called when overlapping area sphere ends
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	                        AActor* OtherActor,
	                        UPrimitiveComponent* OtherComponent,
	                        int32 OtherBodyIndex);

	// set the ActiveStars array of booleans based on rarity
	void SetActiveStarts();

	// sets properties of the item's components based on state
	virtual void SetItemProperties(EItemState State);

	// called when ItemInterpTimer is finished
	void FinishInterping();

	// handles item interpolation when in EquipInterping state
	void ItemInterp(float DeltaTime);

	// get Interp location based in the item type
	FVector GetInterpLocation();

	void PlayPickupSound();
	
	virtual void InitializeCustomDepth();

	/** This function is the equivalent of the construction script tab of the blueprint editor for any BP based on this Item class.*/
	virtual void OnConstruction(const FTransform& Transform) override;

	void EnableGlowMaterial();

	void UpdatePulse();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// skeletal mesh for the item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* ItemMesh;

	// line trace collides with box to show HUD widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class UBoxComponent* CollisionBox;

	// pop up widget for when the player looks at the item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class UWidgetComponent* PickupWidget;

	// enables item tracing when overlapped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class USphereComponent* AreaSphere;

	// name which appears on the pickup widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	FString ItemName;

	// number of items offered
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	int32 ItemCount;

	// item rarity more start means more rare the item is
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	TArray<bool> ActiveStars;

	// this item's state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	EItemState ItemState;

	// the curve asset that is used for the item's Z location when it is interping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class UCurveFloat* ItemZCurve;

	// starting location when interping begins
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	FVector ItemInterpStartLocation;

	// target interp location front of the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	FVector CameraTargetLocation;

	// true when this item is interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	bool bInterping;

	// plays when we start interping so we have a time handle
	FTimerHandle ItemInterpTimer;

	// pointer to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class AShooterCharacter* Character;

	// Duration of the curve and timer (x axis length on the `ItemZCurve` asset)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	float ZCurveTime;

	// X and Y for the item interping in EquipInterpingState
	float ItemInterpX;
	float ItemInterpY;

	// initial yaw offset between the camera and the interping item
	float InterpInitialYawOffset;

	// sound to play when this item is picked up
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class USoundCue* PickupSound;

	// sound to play when this item is equipped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class USoundCue* EquipSound;

	// enum for the type of the item this item is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	EItemType ItemType;

	// Index of the location where an item will interp to upon pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	int32 InterToLocationIndex;

	// index for the material we would like to change at the runtime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	int32 MaterialIndex;

	// Dynamic Material Instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class UMaterialInstanceDynamic* DynamicMaterialInstance;

	// Material Instance that will be used with the Dynamic Material Instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class UMaterialInstance* MaterialInstance;
	
	bool bCanChangeCustomDepth;

	// curve to drive the dynamic material parameters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class UCurveVector* PulseCurve;

	// curve to drive the dynamic material parameters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	class UCurveVector* InterpPulseCurve;

	// time for the pulse timer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	float PulseCurveTime;
	
	FTimerHandle PulseTimer;

	// comes from material instance M_SMG_Mat_Inst
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	float GlowAmount;
	// comes from material instance M_SMG_Mat_Inst
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	float FresnelExponent;
	// comes from material instance M_SMG_Mat_Inst
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	float FresnelReflectFraction;
	void ResetPulseTimer();

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE class USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE class USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }

	void SetItemState(EItemState State);

	// Called from the AShooterCharacter class
	void StartItemCurve(class AShooterCharacter* InteractingCharacter);
	// called in AShooterCharacter::GetPickupItem
	void PlayEquipSound();
	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
	void DisableGlowMaterial();
	void StartPulseTimer();
};
