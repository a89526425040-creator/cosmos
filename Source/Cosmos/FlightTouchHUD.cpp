#include "FlightTouchHUD.h"
#include "SpaceFlightPawn.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UFlightTouchHUD::SetShip(ASpaceFlightPawn* InShip)
{
    Ship = InShip;
}

void UFlightTouchHUD::NativeConstruct()
{
    Super::NativeConstruct();

    RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("TouchRoot"));
    WidgetTree->RootWidget = RootCanvas;

    // The two virtual sticks handle thrust/strafe and pitch/yaw.
    // These four large buttons add the remaining roll and vertical translation axes.
    RollLeftButton = MakeButton(TEXT("RollLeft"), FText::FromString("ROLL -"),
        FAnchors(0.82f, 0.12f, 0.91f, 0.20f));
    RollRightButton = MakeButton(TEXT("RollRight"), FText::FromString("ROLL +"),
        FAnchors(0.92f, 0.12f, 1.0f, 0.20f));
    LiftUpButton = MakeButton(TEXT("LiftUp"), FText::FromString("UP"),
        FAnchors(0.82f, 0.22f, 0.91f, 0.30f));
    LiftDownButton = MakeButton(TEXT("LiftDown"), FText::FromString("DOWN"),
        FAnchors(0.92f, 0.22f, 1.0f, 0.30f));

    RollLeftButton->OnPressed.AddDynamic(this, &UFlightTouchHUD::RollLeftPressed);
    RollRightButton->OnPressed.AddDynamic(this, &UFlightTouchHUD::RollRightPressed);
    LiftUpButton->OnPressed.AddDynamic(this, &UFlightTouchHUD::LiftUpPressed);
    LiftDownButton->OnPressed.AddDynamic(this, &UFlightTouchHUD::LiftDownPressed);

    RollLeftButton->OnReleased.AddDynamic(this, &UFlightTouchHUD::RollReleased);
    RollRightButton->OnReleased.AddDynamic(this, &UFlightTouchHUD::RollReleased);
    LiftUpButton->OnReleased.AddDynamic(this, &UFlightTouchHUD::LiftReleased);
    LiftDownButton->OnReleased.AddDynamic(this, &UFlightTouchHUD::LiftReleased);
}

UButton* UFlightTouchHUD::MakeButton(FName Name, const FText& Label, const FAnchors& Anchors)
{
    UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
    Button->SetBackgroundColor(FLinearColor(0.025f, 0.10f, 0.17f, 0.78f));
    Button->SetIsFocusable(false);

    UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Text->SetText(Label);
    Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    Text->SetJustification(ETextJustify::Center);
    Button->AddChild(Text);

    UCanvasPanelSlot* Slot = RootCanvas->AddChildToCanvas(Button);
    Slot->SetAnchors(Anchors);
    Slot->SetOffsets(FMargin(2.0f));
    Slot->SetZOrder(20);
    return Button;
}

void UFlightTouchHUD::RollLeftPressed()  { if (Ship) Ship->SetTouchRoll(-1.0f); }
void UFlightTouchHUD::RollRightPressed() { if (Ship) Ship->SetTouchRoll(1.0f); }
void UFlightTouchHUD::LiftUpPressed()    { if (Ship) Ship->SetTouchLift(1.0f); }
void UFlightTouchHUD::LiftDownPressed()  { if (Ship) Ship->SetTouchLift(-1.0f); }
void UFlightTouchHUD::RollReleased()     { if (Ship) Ship->SetTouchRoll(0.0f); }
void UFlightTouchHUD::LiftReleased()     { if (Ship) Ship->SetTouchLift(0.0f); }
