#include "CosmosPlayerController.h"
#include "FlightTouchHUD.h"
#include "SpaceFlightPawn.h"
#include "Blueprint/UserWidget.h"

void ACosmosPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (!IsLocalController()) return;

    if (!TouchHUD)
    {
        TouchHUD = CreateWidget<UFlightTouchHUD>(this, UFlightTouchHUD::StaticClass());
        if (TouchHUD)
        {
            TouchHUD->AddToViewport(20);
        }
    }

    if (TouchHUD)
    {
        TouchHUD->SetShip(Cast<ASpaceFlightPawn>(InPawn));
    }

    bEnableTouchEvents = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = false;
    SetShowMouseCursor(false);
    SetInputMode(FInputModeGameAndUI());
}
