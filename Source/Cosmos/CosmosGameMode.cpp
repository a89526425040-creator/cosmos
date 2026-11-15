#include "CosmosGameMode.h"
#include "SpaceFlightPawn.h"
#include "CosmosPlayerController.h"
#include "PlanetActor.h"
#include "StarfieldActor.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"

ACosmosGameMode::ACosmosGameMode()
{
    DefaultPawnClass = ASpaceFlightPawn::StaticClass();
    PlayerControllerClass = ACosmosPlayerController::StaticClass();
}

void ACosmosGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    if (GetWorld())
    {
        const FTransform StartTransform(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 0.0f, 700000.0f));
        GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), StartTransform);
    }
}

void ACosmosGameMode::StartPlay()
{
    Super::StartPlay();

    if (!GetWorld()) return;

    GetWorld()->SpawnActor<APlanetActor>(APlanetActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    GetWorld()->SpawnActor<AStarfieldActor>(AStarfieldActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

    ADirectionalLight* SunLight = GetWorld()->SpawnActor<ADirectionalLight>(
        ADirectionalLight::StaticClass(), FVector::ZeroVector, FRotator(-28.0f, -35.0f, 0.0f));
    if (SunLight && SunLight->GetLightComponent())
    {
        SunLight->GetLightComponent()->SetIntensity(100000.0f);
        SunLight->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.96f, 0.88f));
    }
}

void ACosmosGameMode::RestartPlayer(AController* NewPlayer)
{
    Super::RestartPlayer(NewPlayer);

    if (NewPlayer)
    {
        if (ASpaceFlightPawn* Ship = Cast<ASpaceFlightPawn>(NewPlayer->GetPawn()))
        {
            // Near-circular low orbit at the chosen gameplay scale.
            const double Radius = 700000.0;
            const double OrbitalSpeed = FMath::Sqrt(3.986004418e11 / Radius);
            Ship->SetInitialVelocity(FVector(0.0, OrbitalSpeed, 0.0));
        }
    }
}
