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
	void SetItemProperties(EItemState State);

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
	TArray<bool>ActiveStars;

	// this item's state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=ItemProperties, meta=(AllowPrivateAccess="true"))
	EItemState ItemState;

public:

	FORCEINLINE UWidgetComponent* GetPickupWidget() const {return PickupWidget;}
	FORCEINLINE USphereComponent* GetAreaSphere() const {return AreaSphere;}
	FORCEINLINE UBoxComponent* GetCollisionBox() const {return CollisionBox;}
	FORCEINLINE EItemState GetItemState() const {return ItemState;}
	void SetItemState(EItemState State);
	
};
