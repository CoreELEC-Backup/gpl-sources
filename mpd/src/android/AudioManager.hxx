/*
 * Copyright 2003-2020 The Music Player Daemon Project
 * http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MPD_ANDROID_AUDIO_MANAGER_HXX
#define MPD_ANDROID_AUDIO_MANAGER_HXX

#include "java/Object.hxx"

class AudioManager : public Java::GlobalObject {
	int maxVolume;
	jmethodID getStreamVolumeMethod;
	jmethodID setStreamVolumeMethod;

public:
	AudioManager(JNIEnv *env, jobject obj) noexcept;

	AudioManager(std::nullptr_t) noexcept { maxVolume = 0; }

	~AudioManager() noexcept {}

	int GetMaxVolume() { return maxVolume; }
	int GetVolume(JNIEnv *env);
	void SetVolume(JNIEnv *env, int);
};

#endif
