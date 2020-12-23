#include "stdafx.h"
#include "../Utilities/FolderUtilities.h"
#include "MovieManager.h"
#include "MesenMovie.h"
#include "BizhawkMovie.h"
#include "FceuxMovie.h"
#include "MovieRecorder.h"
#include "VirtualFile.h"

shared_ptr<IMovie> MovieManager::_player;
shared_ptr<MovieRecorder> MovieManager::_recorder;

void MovieManager::Record(RecordMovieOptions options, shared_ptr<Console> console)
{
	shared_ptr<MovieRecorder> recorder(new MovieRecorder(console));
	if(recorder->Record(options)) {
		_recorder = recorder;
	}
}

void MovieManager::Play(VirtualFile file, shared_ptr<Console> console)
{
	vector<uint8_t> fileData;
	if(file.IsValid() && file.ReadFile(fileData)) {
		shared_ptr<IMovie> player;
		if(memcmp(fileData.data(), "MMO", 3) == 0) {
			//Old movie format, no longer supported
			MessageManager::DisplayMessage("Movies", "MovieIncompatibleVersion");
		} else if(memcmp(fileData.data(), "PK", 2) == 0) {
			//Mesen or Bizhawk movie
			ZipReader reader;
			reader.LoadArchive(fileData);

			vector<string> files = reader.GetFileList();
			if(std::find(files.begin(), files.end(), "GameSettings.txt") != files.end()) {
				player.reset(new MesenMovie(console));
			} else {
				player.reset(new BizhawkMovie(console));
			}
		} else if(memcmp(fileData.data(), "ver", 3) == 0) {
			player.reset(new FceuxMovie(console));
		}

		if(player && player->Play(file)) {
			_player = player;

			MessageManager::DisplayMessage("Movies", "MoviePlaying", file.GetFileName());
		}
	}
}

void MovieManager::Stop()
{
	_player.reset();

	if(_recorder) {
		_recorder.reset();
	}
}

bool MovieManager::Playing()
{
	shared_ptr<IMovie> player = _player;
	return player && player->IsPlaying();
}

bool MovieManager::Recording()
{
	return _recorder != nullptr;
}
