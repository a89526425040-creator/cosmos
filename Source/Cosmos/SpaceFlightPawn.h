#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpaceFlightPawn.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class COSMOS_API ASpaceFlightPawn : public APawn
{
    GENERATED_BODY()

public:
    ASpaceFlightPawn();

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void SetInitialVelocity(const FVector& InVelocity);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBoxComponent> CollisionRoot;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Fuselage;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Nose;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> LeftWing;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> RightWing;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Engine;

    FVector Velocity = FVector::ZeroVector;
    FVector AngularVelocityLocal = FVector::ZeroVector;

    float ThrottleInput = 0.0f;
    float PitchInput = 0.0f;
    float YawInput = 0.0f;
    float RollInput = 0.0f;
    float StrafeInput = 0.0f;
    float LiftInput = 0.0f;

    float FuelKg = 6000.0f;
    float DryMassKg = 12000.0f;
    float MainThrustNewtons = 250000.0f;
    float SpecificImpulseSeconds = 450.0f;
    float RcsAcceleration = 0.28f;
    float AngularAcceleration = 0.38f;
    float AngularDamping = 0.12f;

    static constexpr double EarthRadiusCm = 637100.0;
    static constexpr double EarthMuCm3PerSec2 = 3.986004418e11;
    static constexpr double MetresPerGameCentimetre = 10.0;

    void InputThrottle(float Value);
    void InputPitch(float Value);
    void InputYaw(float Value);
    void InputRollPositive();
    void InputRollNegative();
    void InputStrafe(float Value);
    void InputLiftPositive();
    void InputLiftNegative();
    void BuildShipVisuals();
    void SetPartTint(UPrimitiveComponent* Component, const FLinearColor& Color);
    void UpdateFlightPhysics(float DeltaSeconds);
    void UpdateFlightReadout();
};
