#include "audio/SoundSystem.h"
#include <algorithm>

namespace une
{
    void SoundSystem::Update()
    {
        SetListeningPosition(ListeningPosition);

        // Iterate through entities in the system
        for (ecs::Entity entity : entities)
        {
            // Get necessary components
            SoundComponent& soundComponent = ecs::GetComponent<SoundComponent>(entity);
            Transform& soundTransform = ecs::GetComponent<Transform>(entity);

            for (auto& sound : soundComponent.Sounds)
            {
                //calculate distance between listener and audio source
                float distance = (ListeningPosition - soundTransform.position).Length();

                DistanceModel model = soundComponent.distanceModel;

                float attenuation = 1.0f;

                if (distance <= soundComponent.referenceDistance) {
                    attenuation = 1.0f;
                }
                else {
                    switch (model)
                    {
                        case DistanceModel::LINEAR:
                            attenuation = std::max(0.0f, 1.0f - (distance / soundComponent.maxDistance));  // Linear attenuation
                            break;

                        case DistanceModel::INVERSE:
                            attenuation = 1.0f / (1.0f + soundComponent.rolloffFactor * (distance - soundComponent.referenceDistance));  // Inverse attenuation
                            break;

                        case DistanceModel::EXPONENTIAL:
                            attenuation = pow(0.5f, (distance - soundComponent.referenceDistance) / soundComponent.rolloffFactor);  // Exponential attenuation
                            break;
                        case DistanceModel::NONE:
                            break;
                    }
                }

                // Set the audio volume based on the calculated attenuation
                float attenuatedVolume = (soundComponent.baseVolume * sound.second->Volume * attenuation);
                sound.second->absoluteVolume(std::clamp(attenuatedVolume, 0.0f, soundComponent.baseVolume));
                sound.second->setAbsoluteDirection(soundTransform.position - ListeningPosition);
            }
        }
    }

    AudioEngine* SoundSystem::AddSoundEngine(std::string soundEngineName, float ambientVolume)
    {
        auto it = AudioEngines.find(soundEngineName);
        if (it != AudioEngines.end())
        {
            std::cout << "Warning: Sound engine with name " << soundEngineName << " already exists.\n";
            return nullptr;
        }

        AudioEngine* NewAudioEngine = new AudioEngine(ambientVolume);
        AudioEngines[soundEngineName] = NewAudioEngine;
        return NewAudioEngine;
    }

    AudioEngine* SoundSystem::FindAudioEngine(const std::string& soundEngineName)
    {
        auto it = AudioEngines.find(soundEngineName);
        if (it == AudioEngines.end())
        {
            return nullptr;
        }

        return it->second;
    }

    void SoundSystem::SetListeningPosition(Vector3 listeningPosition)
    {
        ListeningPosition = listeningPosition;

        for (auto& audioEngine : AudioEngines)
        {
            audioEngine.second->setListenerPosition(ListeningPosition);
        }
    }

    //Set the global volume percentage
    void SoundSystem::SetGlobalVolume(float volume)
    {
        for (auto& audioEngine : AudioEngines)
        {
            audioEngine.second->setAmbientVolume(volume);
        }
    }

    // Free up memory
    void SoundSystem::Uninitialize()
    {
        for (auto& AudioEngineToDelete : AudioEngines)
        {
            delete AudioEngineToDelete.second;
        }
    }

    Audio* AddAudio(std::string AudioEngineName, std::string AudioFile, bool loop, float volume, DistanceModel model, float maxDist, float refDist, float rolloff)
    {
        auto* audio = ecs::GetSystem<une::SoundSystem>()->FindAudioEngine(AudioEngineName)->createAudio(AudioFile, loop, volume);

        if (audio)
        {
            audio->pause();
            audio->setVolume(volume);
            audio->setDistanceModel(model);
            audio->setMaxDistance(maxDist);
            audio->setReferenceDistance(refDist);
            audio->setRolloffFactor(rolloff);
        }
        return audio;
    }
}