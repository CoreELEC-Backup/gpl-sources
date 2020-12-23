#pragma once
#include "stdafx.h"
#include "../Utilities/ZipReader.h"
#include "HdData.h"
#include "VirtualFile.h"

class HdPackLoader
{
public:
	static bool LoadHdNesPack(string definitionFile, HdPackData &outData);
	static bool LoadHdNesPack(VirtualFile &romFile, HdPackData &outData);

private:
	HdPackData* _data;
	bool _loadFromZip = false;
	ZipReader _reader;
	string _hdPackDefinitionFile;
	string _hdPackFolder;
	vector<HdPackBitmapInfo> _hdNesBitmaps;

	HdPackLoader();

	bool InitializeLoader(VirtualFile &romPath, HdPackData *data);
	bool LoadFile(string filename, vector<uint8_t> &fileData);
	bool CheckFile(string filename);

	bool LoadPack();
	void InitializeHdPack();
	void LoadCustomPalette();

	void InitializeGlobalConditions();

	//Video
	bool ProcessImgTag(string src);
	void PremultiplyAlpha(vector<uint32_t>& pixelData);
	void ProcessPatchTag(vector<string> &tokens);
	void ProcessOverscanTag(vector<string> &tokens);
	void ProcessConditionTag(vector<string> &tokens, bool createInvertedCondition);
	void ProcessTileTag(vector<string> &tokens, vector<HdPackCondition*> conditions);
	void ProcessBackgroundTag(vector<string> &tokens, vector<HdPackCondition*> conditions);
	void ProcessOptionTag(vector<string>& tokens);

	//Audio
	int ProcessSoundTrack(string albumString, string trackString, string filename);
	void ProcessBgmTag(vector<string> &tokens);
	void ProcessSfxTag(vector<string> &tokens);

	vector<HdPackCondition*> ParseConditionString(string conditionString, vector<unique_ptr<HdPackCondition>> &conditions);
};