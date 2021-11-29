// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrazyKartsOnlineGameMode.h"
#include "CrazyKartsOnlinePawn.h"
#include "CrazyKartsOnlineHud.h"

ACrazyKartsOnlineGameMode::ACrazyKartsOnlineGameMode()
{
	DefaultPawnClass = ACrazyKartsOnlinePawn::StaticClass();
	HUDClass = ACrazyKartsOnlineHud::StaticClass();
}
