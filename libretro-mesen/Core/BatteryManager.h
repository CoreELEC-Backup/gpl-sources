#pragma once
#include "stdafx.h"

class IBatteryProvider
{
public:
	virtual vector<uint8_t> LoadBattery(string extension) = 0;
};

class IBatteryRecorder
{
public:
	virtual void OnLoadBattery(string extension, vector<uint8_t> batteryData) = 0;
};

class BatteryManager
{
private:
	string _romName;
	bool _saveEnabled;
	string GetBasePath();

	std::weak_ptr<IBatteryProvider> _provider;
	std::weak_ptr<IBatteryRecorder> _recorder;

public:
	void Initialize(string romName);

	void SetSaveEnabled(bool enabled);
	
	void SetBatteryProvider(shared_ptr<IBatteryProvider> provider);
	void SetBatteryRecorder(shared_ptr<IBatteryRecorder> recorder);
	
	void SaveBattery(string extension, uint8_t* data, uint32_t length);
	
	vector<uint8_t> LoadBattery(string extension);
	void LoadBattery(string extension, uint8_t* data, uint32_t length);
};