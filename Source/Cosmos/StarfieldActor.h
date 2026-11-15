#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StarfieldActor.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class COSMOS_API AStarfieldActor : public AActor
{
    GENERATED_BODY()

public:
    AStarfieldActor();

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Stars;
};
