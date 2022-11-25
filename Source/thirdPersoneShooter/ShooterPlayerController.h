// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONESHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterPlayerController();

protected:
	virtual void BeginPlay() override;

private:
	// reference to the overall HUD overlay blueprint class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Widgets, meta=(AllowPrivateAccess="true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	// variable to hold the HUD overlay widget after creating it
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Widgets, meta=(AllowPrivateAccess="true"))
	class UUserWidget* HUDOverlay;
	
};
