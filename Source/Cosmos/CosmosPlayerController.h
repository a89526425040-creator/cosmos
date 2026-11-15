#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CosmosPlayerController.generated.h"

class UFlightTouchHUD;

UCLASS()
class COSMOS_API ACosmosPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void OnPossess(APawn* InPawn) override;

private:
    UPROPERTY()
    TObjectPtr<UFlightTouchHUD> TouchHUD;
};
