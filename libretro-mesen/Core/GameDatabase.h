#pragma once
#include "stdafx.h"
#include <unordered_map>
#include "RomData.h"

class GameDatabase
{
private:
	static std::unordered_map<uint32_t, GameInfo> _gameDatabase;
	static bool _enabled;

	template<typename T> static T ToInt(string value);

	static BusConflictType GetBusConflictType(string busConflictSetting);
	static GameSystem GetGameSystem(string system);
	static uint8_t GetSubMapper(GameInfo &info);

	static void InitDatabase();
	static void UpdateRomData(GameInfo &info, RomData &romData);
	static void LoadGameDb(vector<string> data);

public:
	static void LoadGameDb(std::istream & db);
	
	static void SetGameDatabaseState(bool enabled);
	static bool IsEnabled();

	static void SetGameInfo(uint32_t romCrc, RomData &romData, bool updateRomData, bool forHeaderlessRom);
	static bool GetiNesHeader(uint32_t romCrc, NESHeader &nesHeader);
	static bool GetDbRomSize(uint32_t romCrc, uint32_t &prgSize, uint32_t &chrSize);
};