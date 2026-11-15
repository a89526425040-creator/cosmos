#include "SpaceFlightPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

ASpaceFlightPawn::ASpaceFlightPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    CollisionRoot = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionRoot"));
    SetRootComponent(CollisionRoot);
    CollisionRoot->SetBoxExtent(FVector(650.0f, 210.0f, 180.0f));
    CollisionRoot->SetCollisionProfileName(TEXT("Pawn"));
    CollisionRoot->SetSimulatePhysics(false);

    Fuselage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fuselage"));
    Fuselage->SetupAttachment(CollisionRoot);
    Nose = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Nose"));
    Nose->SetupAttachment(CollisionRoot);
    LeftWing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWing"));
    LeftWing->SetupAttachment(CollisionRoot);
    RightWing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWing"));
    RightWing->SetupAttachment(CollisionRoot);
    Engine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainEngine"));
    Engine->SetupAttachment(CollisionRoot);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(CollisionRoot);
    CameraBoom->TargetArmLength = 4200.0f;
    CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 700.0f);
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 4.0f;
    CameraBoom->bEnableCameraRotationLag = true;
    CameraBoom->CameraRotationLagSpeed = 5.0f;
    CameraBoom->bDoCollisionTest = false;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->FieldOfView = 85.0f;

    BuildShipVisuals();
}

void ASpaceFlightPawn::BuildShipVisuals()
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

    if (CubeMesh.Succeeded())
    {
        Fuselage->SetStaticMesh(CubeMesh.Object);
        Fuselage->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        Fuselage->SetRelativeScale3D(FVector(9.0f, 1.35f, 1.25f));

        LeftWing->SetStaticMesh(CubeMesh.Object);
        LeftWing->SetRelativeLocation(FVector(-30.0f, -330.0f, -15.0f));
        LeftWing->SetRelativeRotation(FRotator(0.0f, 0.0f, -8.0f));
        LeftWing->SetRelativeScale3D(FVector(3.2f, 5.5f, 0.22f));

        RightWing->SetStaticMesh(CubeMesh.Object);
        RightWing->SetRelativeLocation(FVector(-30.0f, 330.0f, -15.0f));
        RightWing->SetRelativeRotation(FRotator(0.0f, 0.0f, 8.0f));
        RightWing->SetRelativeScale3D(FVector(3.2f, 5.5f, 0.22f));
    }

    if (ConeMesh.Succeeded())
    {
        Nose->SetStaticMesh(ConeMesh.Object);
        Nose->SetRelativeLocation(FVector(500.0f, 0.0f, 0.0f));
        Nose->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
        Nose->SetRelativeScale3D(FVector(1.1f, 1.1f, 3.0f));
    }

    if (CylinderMesh.Succeeded())
    {
        Engine->SetStaticMesh(CylinderMesh.Object);
        Engine->SetRelativeLocation(FVector(-520.0f, 0.0f, 0.0f));
        Engine->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
        Engine->SetRelativeScale3D(FVector(0.9f, 0.9f, 2.0f));
    }

    SetPartTint(Fuselage, FLinearColor(0.72f, 0.78f, 0.84f));
    SetPartTint(Nose, FLinearColor(0.12f, 0.55f, 0.9f));
    SetPartTint(LeftWing, FLinearColor(0.18f, 0.24f, 0.32f));
    SetPartTint(RightWing, FLinearColor(0.18f, 0.24f, 0.32f));
    SetPartTint(Engine, FLinearColor(0.95f, 0.35f, 0.08f));
}

void ASpaceFlightPawn::SetPartTint(UPrimitiveComponent* Component, const FLinearColor& Color)
{
    if (!Component) return;

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BasicMaterial(
        TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    UMaterialInterface* Parent = BasicMaterial.Succeeded()
        ? BasicMaterial.Object
        : UMaterial::GetDefaultMaterial(MD_Surface);

    UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Parent, this);
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
        DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
        Component->SetMaterial(0, DynamicMaterial);
    }
    Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Component->SetCastShadow(true);
}

void ASpaceFlightPawn::BeginPlay()
{
    Super::BeginPlay();
    UpdateFlightReadout();
}

void ASpaceFlightPawn::SetInitialVelocity(const FVector& InVelocity)
{
    Velocity = InVelocity;
}

void ASpaceFlightPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("Throttle"), this, &ASpaceFlightPawn::InputThrottle);
    PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &ASpaceFlightPawn::InputPitch);
    PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &ASpaceFlightPawn::InputYaw);
    PlayerInputComponent->BindAxis(TEXT("Roll"), this, &ASpaceFlightPawn::InputRollPositive);
    PlayerInputComponent->BindAxis(TEXT("Strafe"), this, &ASpaceFlightPawn::InputStrafe);
    PlayerInputComponent->BindAxis(TEXT("Lift"), this, &ASpaceFlightPawn::InputLiftPositive);

    PlayerInputComponent->BindAction(TEXT("RollLeft"), IE_Pressed, this, &ASpaceFlightPawn::InputRollNegative);
    PlayerInputComponent->BindAction(TEXT("RollRight"), IE_Pressed, this, &ASpaceFlightPawn::InputRollPositive);
    PlayerInputComponent->BindAction(TEXT("LiftUp"), IE_Pressed, this, &ASpaceFlightPawn::InputLiftPositive);
    PlayerInputComponent->BindAction(TEXT("LiftDown"), IE_Pressed, this, &ASpaceFlightPawn::InputLiftNegative);
}

void ASpaceFlightPawn::InputThrottle(float Value) { ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f); }
void ASpaceFlightPawn::InputPitch(float Value) { PitchInput = FMath::Clamp(Value, -1.0f, 1.0f); }
void ASpaceFlightPawn::InputYaw(float Value) { YawInput = FMath::Clamp(Value, -1.0f, 1.0f); }
void ASpaceFlightPawn::InputRollPositive() { RollInput = 1.0f; }
void ASpaceFlightPawn::InputRollNegative() { RollInput = -1.0f; }
void ASpaceFlightPawn::InputStrafe(float Value) { StrafeInput = FMath::Clamp(Value, -1.0f, 1.0f); }
void ASpaceFlightPawn::InputLiftPositive() { LiftInput = 1.0f; }
void ASpaceFlightPawn::InputLiftNegative() { LiftInput = -1.0f; }

void ASpaceFlightPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (DeltaSeconds <= 0.0f) return;

    UpdateFlightPhysics(FMath::Min(DeltaSeconds, 0.05f));
    UpdateFlightReadout();
}

void ASpaceFlightPawn::UpdateFlightPhysics(float DeltaSeconds)
{
    const FVector Position = GetActorLocation();
    const double Radius = Position.Size();
    const FVector RadialUp = Position.GetSafeNormal();

    FVector Acceleration = FVector::ZeroVector;
    if (Radius > EarthRadiusCm * 0.25)
    {
        Acceleration -= RadialUp * (EarthMuCm3PerSec2 / (Radius * Radius));
    }

    const float CurrentMass = DryMassKg + FuelKg;
    const float SignedThrottle = ThrottleInput;
    if (FuelKg > 0.0f && FMath::Abs(SignedThrottle) > KINDA_SMALL_NUMBER)
    {
        const float Thrust = SignedThrottle >= 0.0f
            ? MainThrustNewtons
            : MainThrustNewtons * 0.22f;

        // The game uses a compressed solar-system scale: 100 game cm = 1 km.
        // Therefore 1 m/s^2 of physical acceleration equals 0.1 game cm/s^2.
        const float Accel = (Thrust / CurrentMass) * 0.1f;
        Acceleration += GetActorForwardVector() * (Accel * SignedThrottle);

        const float FuelFlowKgPerSecond = Thrust / (SpecificImpulseSeconds * 9.80665f);
        FuelKg = FMath::Max(0.0f, FuelKg - FuelFlowKgPerSecond * FMath::Abs(SignedThrottle) * DeltaSeconds);
    }

    Acceleration += GetActorRightVector() * (StrafeInput * RcsAcceleration);
    Acceleration += GetActorUpVector() * (LiftInput * RcsAcceleration);

    Velocity += Acceleration * DeltaSeconds;

    // Tiny numerical damping only. In vacuum, momentum is preserved when controls are released.
    Velocity *= FMath::Exp(-0.000015f * DeltaSeconds);

    FHitResult Hit;
    SetActorLocation(GetActorLocation() + Velocity * DeltaSeconds, true, &Hit, ETeleportType::None);
    if (Hit.bBlockingHit)
    {
        const float IntoSurface = FVector::DotProduct(Velocity, Hit.ImpactNormal);
        if (IntoSurface < 0.0f)
        {
            Velocity -= Hit.ImpactNormal * IntoSurface;
        }
    }

    const FVector AngularAccelerationVector(RollInput, PitchInput, YawInput);
    AngularVelocityLocal += AngularAccelerationVector * AngularAcceleration * DeltaSeconds;
    AngularVelocityLocal *= FMath::Exp(-AngularDamping * DeltaSeconds);

    const float AngularSpeed = AngularVelocityLocal.Size();
    if (AngularSpeed > KINDA_SMALL_NUMBER)
    {
        const FQuat DeltaRotation(AngularVelocityLocal / AngularSpeed, AngularSpeed * DeltaSeconds);
        AddActorLocalRotation(DeltaRotation, false, nullptr, ETeleportType::None);
    }

    // Momentary digital controls return to neutral after each frame.
    if (!IsInputKeyDown(EKeys::Q) && !IsInputKeyDown(EKeys::E)) RollInput = 0.0f;
    if (!IsInputKeyDown(EKeys::R) && !IsInputKeyDown(EKeys::F)) LiftInput = 0.0f;
}

void ASpaceFlightPawn::UpdateFlightReadout()
{
    if (!GEngine) return;

    const double Distance = GetActorLocation().Size();
    const double AltitudeKm = FMath::Max(0.0, (Distance - EarthRadiusCm) / 100.0);
    const double SpeedKmPerSecond = Velocity.Size() * 0.01;
    const double SpeedMetresPerSecond = Velocity.Size() * MetresPerGameCentimetre;
    const double AngularSpeedDeg = FMath::RadiansToDegrees(AngularVelocityLocal.Size());

    const FString Readout = FString::Printf(
        TEXT("COSMOS // FLIGHT DATA\nALTITUDE  %.1f km   SPEED  %.3f km/s (%.0f m/s)\n")
        TEXT("THROTTLE  %+.0f%%   FUEL  %.0f / 6000 kg\nANGULAR RATE  %.1f deg/s\n")
        TEXT("W/S: THRUST  A/D: YAW  MOUSE: PITCH/LOOK  Q/E: ROLL  R/F: VERTICAL RCS  J/L: STRAFE"),
        AltitudeKm, SpeedKmPerSecond, SpeedMetresPerSecond,
        ThrottleInput * 100.0f, FuelKg, AngularSpeedDeg);

    GEngine->AddOnScreenDebugMessage(100, 0.1f, FColor::Cyan, Readout);
}
