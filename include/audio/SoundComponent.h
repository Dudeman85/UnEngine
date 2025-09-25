#pragma once

#include <vector>
#include <map>

#include "ECS.h"
#include "Transform.h"
#include "audio/AudioEngine.h"

namespace une
{
	ECS_REGISTER_COMPONENT(SoundComponent)
	struct SoundComponent
	{
		std::map<std::string, Audio*> Sounds;
		DistanceModel distanceModel = DistanceModel::LINEAR; // default
		float maxDistance = 1500.0f;
		float referenceDistance = 200.0f;
		float rolloffFactor = 1.0f;
		float baseVolume = 1.0f;

		std::unordered_map<std::string, float> startDelays; // Delay in milliseconds for each sound
		std::unordered_map<std::string, float> stopDelays;  // Stop time in milliseconds for each sound

		bool deleteSounds = false;
	};

	ECS_REGISTER_SYSTEM(SoundSystem, SoundComponent, Transform)
	class SoundSystem : public ecs::System
	{
	private:
		std::map<std::string, AudioEngine*> AudioEngines;

	public:
		Vector3 ListeningPosition;

		void Update();

		AudioEngine* AddSoundEngine(std::string soundEngineName, float ambientVolume = 1);
		AudioEngine* FindAudioEngine(const std::string& soundEngineName);
		void SetListeningPosition(Vector3 listeningPosition);

		//Set the global volume percentage
		void SetGlobalVolume(float volume);
		// Free up memory
		void Uninitialize();
	};

	Audio* AddAudio(std::string AudioEngineName, std::string AudioFile, bool loop, float volume, DistanceModel model, float maxDist = 1000.0f, float refDist = 1.0f, float rolloff = 1.0f);
};
