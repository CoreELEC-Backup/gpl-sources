#pragma once
#include "stdafx.h"
#include <thread>
#include "../Utilities/AutoResetEvent.h"
#include "../Utilities/IVideoRecorder.h"
#include "FrameInfo.h"

class IRenderingDevice;
class AviRecorder;
class Console;
enum class VideoCodec;

class VideoRenderer
{
private:
	shared_ptr<Console> _console;

	AutoResetEvent _waitForRender;
	unique_ptr<std::thread> _renderThread;
	IRenderingDevice* _renderer = nullptr;
	atomic<bool> _stopFlag;

	shared_ptr<IVideoRecorder> _recorder;

	void RenderThread();

public:
	VideoRenderer(shared_ptr<Console> console);
	~VideoRenderer();

	void StartThread();
	void StopThread();

	void UpdateFrame(void *frameBuffer, uint32_t width, uint32_t height);
	void RegisterRenderingDevice(IRenderingDevice *renderer);
	void UnregisterRenderingDevice(IRenderingDevice *renderer);

	void StartRecording(string filename, VideoCodec codec, uint32_t compressionLevel);
	void AddRecordingSound(int16_t* soundBuffer, uint32_t sampleCount, uint32_t sampleRate);
	void StopRecording();
	bool IsRecording();
};