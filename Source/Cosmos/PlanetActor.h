#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlanetActor.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class COSMOS_API APlanetActor : public AActor
{
    GENERATED_BODY()

public:
    APlanetActor();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> PlanetMesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Continents;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> CloudPatches;

    void BuildSurfaceDetails();
    void SetTint(UPrimitiveComponent* Component, const FLinearColor& Color);
};
