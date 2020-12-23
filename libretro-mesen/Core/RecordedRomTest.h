#pragma once

#include "stdafx.h"
#include <deque>
#include "INotificationListener.h"
#include "../Utilities/AutoResetEvent.h"

class VirtualFile;
class Console;

class RecordedRomTest : public INotificationListener
{
private:
	shared_ptr<Console> _console;

	bool _recording;
	bool _runningTest;
	int _badFrameCount;
	bool _recordingFromMovie;

	uint8_t _previousHash[16];
	std::deque<uint8_t*> _screenshotHashes;
	std::deque<uint8_t> _repetitionCount;
	uint8_t _currentCount;
	
	//Used when making a test out of an existing movie/test
	vector<uint8_t> _movieData;
	stringstream _romStream;

	string _filename;
	ofstream _file;

	AutoResetEvent _signal;

private:
	void Reset();
	void ValidateFrame(uint16_t* ppuFrameBuffer);
	void SaveFrame(uint16_t* ppuFrameBuffer);
	void Save();

public:
	RecordedRomTest(shared_ptr<Console> console);
	virtual ~RecordedRomTest();

	void ProcessNotification(ConsoleNotificationType type, void* parameter) override;
	void Record(string filename, bool reset);
	void RecordFromMovie(string testFilename, VirtualFile movieFile);
	void RecordFromTest(string newTestFilename, string existingTestFilename);
	int32_t Run(string filename);
	void Stop();
};