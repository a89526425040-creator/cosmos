#include "StarfieldActor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AStarfieldActor::AStarfieldActor()
{
    PrimaryActorTick.bCanEverTick = false;
    Stars = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Stars"));
    SetRootComponent(Stars);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMesh.Succeeded())
    {
        Stars->SetStaticMesh(SphereMesh.Object);
    }
    Stars->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Stars->SetCastShadow(false);

    FRandomStream Random(42017);
    for (int32 Index = 0; Index < 1600; ++Index)
    {
        const float Z = Random.FRandRange(-1.0f, 1.0f);
        const float Angle = Random.FRandRange(0.0f, 2.0f * PI);
        const float Ring = FMath::Sqrt(1.0f - Z * Z);
        const FVector Direction(Ring * FMath::Cos(Angle), Ring * FMath::Sin(Angle), Z);
        const float Distance = Random.FRandRange(4500000.0f, 18000000.0f);
        const float Size = Random.FRandRange(0.10f, 0.34f);
        Stars->AddInstance(FTransform(FRotator::ZeroRotator, Direction * Distance, FVector(Size)));
    }
}
