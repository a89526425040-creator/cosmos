#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlightTouchHUD.generated.h"

class UCanvasPanel;
class UButton;
class ASpaceFlightPawn;

UCLASS()
class COSMOS_API UFlightTouchHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetShip(ASpaceFlightPawn* InShip);

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY()
    TObjectPtr<UCanvasPanel> RootCanvas;

    UPROPERTY()
    TObjectPtr<ASpaceFlightPawn> Ship;

    UPROPERTY()
    TObjectPtr<UButton> RollLeftButton;

    UPROPERTY()
    TObjectPtr<UButton> RollRightButton;

    UPROPERTY()
    TObjectPtr<UButton> LiftUpButton;

    UPROPERTY()
    TObjectPtr<UButton> LiftDownButton;

    UButton* MakeButton(FName Name, const FText& Label, const FAnchors& Anchors);

    UFUNCTION()
    void RollLeftPressed();
    UFUNCTION()
    void RollRightPressed();
    UFUNCTION()
    void LiftUpPressed();
    UFUNCTION()
    void LiftDownPressed();

    UFUNCTION()
    void RollReleased();
    UFUNCTION()
    void LiftReleased();
};
