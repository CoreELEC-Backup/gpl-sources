#pragma once
#include "stdafx.h"
#include "EmulationSettings.h"
#include "Snapshotable.h"
#include "ControlManager.h"
#include "ControlDeviceState.h"
#include "../Utilities/SimpleLock.h"

class Console;

class BaseControlDevice : public Snapshotable
{
private:
	ControlDeviceState _state;

protected:
	shared_ptr<Console> _console;
	vector<KeyMapping> _keyMappings;
	bool _strobe;
	uint8_t _port;
	SimpleLock _stateLock;

	virtual void RefreshStateBuffer() { }
	virtual void StreamState(bool saving);
	
	void EnsureCapacity(int32_t minBitCount);
	uint32_t GetByteIndex(uint8_t bit);
	virtual bool HasCoordinates();
	virtual bool IsRawString();

	bool IsCurrentPort(uint16_t addr);
	bool IsExpansionDevice();
	void StrobeProcessRead();
	void StrobeProcessWrite(uint8_t value);

	virtual string GetKeyNames() { return ""; }

	void SetPressedState(uint8_t bit, uint32_t keyCode);
	void SetPressedState(uint8_t bit, bool enabled);

	void SetCoordinates(MousePosition pos);

	void SetMovement(MouseMovement mov);
	MouseMovement GetMovement();
	
	virtual void InternalSetStateFromInput();
	
public:
	static constexpr uint8_t ExpDevicePort = 4;
	static constexpr uint8_t ConsoleInputPort = 5;
	static constexpr uint8_t MapperInputPort = 6;
	static constexpr uint8_t ExpDevicePort2 = 7;
	static constexpr uint8_t PortCount = ExpDevicePort2 + 1;

	BaseControlDevice(shared_ptr<Console> console, uint8_t port, KeyMappingSet keyMappingSet = KeyMappingSet());
	virtual ~BaseControlDevice();

	uint8_t GetPort();

	bool IsPressed(uint8_t bit);
	MousePosition GetCoordinates();
	
	virtual bool IsKeyboard();

	void ClearState();
	void SetBit(uint8_t bit);
	void ClearBit(uint8_t bit);
	void InvertBit(uint8_t bit);
	void SetBitValue(uint8_t bit, bool set);
	
	void SetTextState(string state);
	string GetTextState();

	void SetStateFromInput();
	virtual void OnAfterSetState() { }
	
	void SetRawState(ControlDeviceState state);
	ControlDeviceState GetRawState();

	virtual uint8_t ReadRAM(uint16_t addr) = 0;
	virtual void WriteRAM(uint16_t addr, uint8_t value) = 0;
	
	void static SwapButtons(shared_ptr<BaseControlDevice> state1, uint8_t button1, shared_ptr<BaseControlDevice> state2, uint8_t button2);
};
