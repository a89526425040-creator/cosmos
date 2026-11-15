#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CosmosGameMode.generated.h"

UCLASS()
class COSMOS_API ACosmosGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACosmosGameMode();

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void StartPlay() override;
    virtual void RestartPlayer(AController* NewPlayer) override;
};
