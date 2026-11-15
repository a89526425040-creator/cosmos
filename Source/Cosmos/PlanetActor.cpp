#include "PlanetActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace CosmosScale
{
    // Gameplay scale: 100 Unreal centimetres represent 1 real kilometre.
    constexpr float PlanetRadius = 637100.0f;
}

APlanetActor::APlanetActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));

    PlanetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Earth"));
    PlanetMesh->SetupAttachment(SceneRoot);
    if (SphereMesh.Succeeded())
    {
        PlanetMesh->SetStaticMesh(SphereMesh.Object);
    }
    PlanetMesh->SetRelativeScale3D(FVector(CosmosScale::PlanetRadius / 50.0f));
    PlanetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    PlanetMesh->SetCollisionProfileName(TEXT("BlockAll"));
    PlanetMesh->SetGenerateOverlapEvents(false);

    Continents = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Continents"));
    Continents->SetupAttachment(SceneRoot);
    if (SphereMesh.Succeeded())
    {
        Continents->SetStaticMesh(SphereMesh.Object);
    }
    Continents->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CloudPatches = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("CloudPatches"));
    CloudPatches->SetupAttachment(SceneRoot);
    if (SphereMesh.Succeeded())
    {
        CloudPatches->SetStaticMesh(SphereMesh.Object);
    }
    CloudPatches->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APlanetActor::BeginPlay()
{
    Super::BeginPlay();

    SetTint(PlanetMesh, FLinearColor(0.035f, 0.19f, 0.48f, 1.0f));
    SetTint(Continents, FLinearColor(0.12f, 0.38f, 0.16f, 1.0f));
    SetTint(CloudPatches, FLinearColor(0.78f, 0.86f, 0.92f, 1.0f));
    BuildSurfaceDetails();
}

void APlanetActor::SetTint(UPrimitiveComponent* Component, const FLinearColor& Color)
{
    if (!Component) return;

    UMaterialInterface* Parent = Component->GetMaterial(0);
    if (!Parent)
    {
        Parent = LoadObject<UMaterialInterface>(nullptr,
            TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
        if (!Parent) Parent = UMaterial::GetDefaultMaterial(MD_Surface);
    }

    UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Parent, this);
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
        DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
        Component->SetMaterial(0, DynamicMaterial);
    }
}

void APlanetActor::BuildSurfaceDetails()
{
    // Procedural, lightweight surface patches. These are visual details, not a GIS terrain map.
    const float R = CosmosScale::PlanetRadius;
    const FVector LandScale(R * 0.075f / 50.0f, R * 0.11f / 50.0f, R * 0.018f / 50.0f);

    const FVector Directions[] =
    {
        FVector(0.25f, 0.55f, 0.79f), FVector(-0.52f, 0.18f, 0.83f),
        FVector(0.62f, -0.46f, 0.63f), FVector(-0.72f, -0.48f, 0.50f),
        FVector(0.05f, -0.92f, 0.39f), FVector(0.78f, 0.18f, -0.60f),
        FVector(-0.32f, 0.80f, -0.50f), FVector(-0.85f, 0.12f, -0.50f),
        FVector(0.36f, 0.55f, -0.75f), FVector(0.05f, -0.35f, -0.93f),
        FVector(-0.48f, -0.72f, -0.50f), FVector(0.86f, -0.20f, -0.46f)
    };

    for (const FVector& RawDirection : Directions)
    {
        const FVector Normal = RawDirection.GetSafeNormal();
        const FQuat Rotation = FRotationMatrix::MakeFromZ(Normal).ToQuat();
        const FVector Location = Normal * (R * 0.999f);
        Continents->AddInstance(FTransform(Rotation, Location, LandScale));
    }

    // Small high-altitude cloud clusters.
    FRandomStream Random(20261115);
    for (int32 Index = 0; Index < 34; ++Index)
    {
        const float Z = Random.FRandRange(-1.0f, 1.0f);
        const float Angle = Random.FRandRange(0.0f, 2.0f * PI);
        const float Ring = FMath::Sqrt(1.0f - Z * Z);
        const FVector Normal(Ring * FMath::Cos(Angle), Ring * FMath::Sin(Angle), Z);
        const FVector Location = Normal * (R * 1.012f);
        const FQuat Rotation = FRotationMatrix::MakeFromZ(Normal).ToQuat();
        const float Size = Random.FRandRange(0.012f, 0.028f) * R / 50.0f;
        CloudPatches->AddInstance(FTransform(
            Rotation, Location, FVector(Size * 1.8f, Size, Size * 0.24f)));
    }
}
