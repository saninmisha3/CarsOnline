// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "CrazyKartsOnlineHud.generated.h"


UCLASS(config = Game)
class ACrazyKartsOnlineHud : public AHUD
{
	GENERATED_BODY()

public:
	ACrazyKartsOnlineHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
