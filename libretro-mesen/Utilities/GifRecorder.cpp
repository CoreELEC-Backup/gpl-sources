#include "stdafx.h"
#include "GifRecorder.h"
#include "gif.h"

GifRecorder::GifRecorder()
{
	_gif.reset(new GifWriter());
}

GifRecorder::~GifRecorder()
{
	StopRecording();
}

bool GifRecorder::StartRecording(string filename, uint32_t width, uint32_t height, uint32_t bpp, uint32_t audioSampleRate, double fps)
{
	_outputFile = filename;
	_recording = GifBegin(_gif.get(), filename.c_str(), width, height, 2, 8, false);
	_frameCounter = 0;
	return _recording;
}

void GifRecorder::StopRecording()
{
	if(_recording) {
		GifEnd(_gif.get());
	}
}

void GifRecorder::AddFrame(void* frameBuffer, uint32_t width, uint32_t height, double fps)
{
	_frameCounter++;
	
	if(fps < 55 || (_frameCounter % 6) != 0) {
		//At 60 FPS, skip 1 of every 6 frames (max FPS for GIFs is 50fps)
		GifWriteFrame(_gif.get(), (uint8_t*)frameBuffer, width, height, 2, 8, false);
	}
}

void GifRecorder::AddSound(int16_t* soundBuffer, uint32_t sampleCount, uint32_t sampleRate)
{
}

bool GifRecorder::IsRecording()
{
	return _recording;
}

string GifRecorder::GetOutputFile()
{
	return _outputFile;
}