// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

    void UShooterAnimInstance::NativeInitializeAnimation()
    {
        ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
    }


	void UShooterAnimInstance::UpdateAnimationProperties(float deltaTime)
    {
       if(ShooterCharacter == nullptr)
       {
            ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
       }

       if(ShooterCharacter)
       {    
            // get leteral speed of the character from its velocity
            FVector velocity = ShooterCharacter->GetVelocity();
            velocity.Z = 0;
            Speed = velocity.Size();

            // is the character in the air?
            bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

            // is the character accelerating?
            // here we will use the oversimplification that acceleration = movement
            // I know that is wrong, but is works good enough for our purposes without complicating things
            bIsAccelrating = (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0);

            const FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
            const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
            MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
       		if (ShooterCharacter->GetVelocity().Size() > 0.f)
       		{
       			LastMovementOffsetYaw = MovementOffsetYaw;
       		}


       		// is the character aiming?
       		bAiming = ShooterCharacter->GetAiming();

            /*
            FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), AimRotation.Yaw);
            FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);
            
            FString OffsetYawMessage = FString::Printf(TEXT("Offset Yaw: %f"), MovementOffsetYaw);
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green, OffsetYawMessage);
            }*/

            
            
       }
    }

