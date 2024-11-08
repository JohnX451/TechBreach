// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "TechUtilLib.h"
#include "HardwareInfo.h"

FString UTechUtilLib::GetProjectVersion()
{
	FString Version;
	GConfig->GetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), Version, GGameIni);
	return Version;
}

FString UTechUtilLib::GetRhiName()
{
	return FHardwareInfo::GetHardwareInfo(NAME_RHI);
}
