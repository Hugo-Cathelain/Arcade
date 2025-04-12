///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Arcade/audio/miniaudio.h"
#include <string>
#include <thread>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Audio playback system based on miniaudio with optimized performance
///
///////////////////////////////////////////////////////////////////////////////
class Audio
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Maximum number of concurrent sounds
    ///////////////////////////////////////////////////////////////////////////
    static constexpr size_t MAX_SOUNDS = 32;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sound status
    ///////////////////////////////////////////////////////////////////////////
    enum class SoundStatus {
        PLAYING,    ///< Sound is currently playing
        STOPPED,    ///< Sound is stopped
        PAUSED      ///< Sound is paused
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Audio source data
    ///////////////////////////////////////////////////////////////////////////
    struct AudioSource {
        std::string path;         ///< Path to the audio file
        ma_decoder decoder;       ///< Audio decoder
        std::atomic<int> refCount;///< Reference counter for cache management

        AudioSource() : refCount(0) {}
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sound instance for playback
    ///////////////////////////////////////////////////////////////////////////
    struct Sound {
        std::string id;                  ///< Unique identifier
        std::shared_ptr<AudioSource> source; ///< Audio source data
        ma_uint64 position;              ///< Current playback position (in frames)
        float volume;                    ///< Playback volume (0.0 to 1.0)
        bool loop;                       ///< Whether to loop the sound
        SoundStatus status;              ///< Current status

        Sound() : position(0), volume(1.0f), loop(false), status(SoundStatus::STOPPED) {}
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Static members
    ///////////////////////////////////////////////////////////////////////////
    static std::unordered_map<std::string, std::shared_ptr<AudioSource>> mSources;
    static std::array<Sound, MAX_SOUNDS> mSoundPool;
    static size_t mActiveSounds;
    static ma_device mDevice;
    static std::mutex mMutex;
    static std::atomic<bool> mInitialized;
    static std::vector<float> mMixBuffer;
    static ma_format mOutputFormat;
    static ma_uint32 mOutputChannels;
    static ma_uint32 mSampleRate;

    ///////////////////////////////////////////////////////////////////////////
    // Static methods
    ///////////////////////////////////////////////////////////////////////////
    static void DataCallback(
        ma_device* pDevice,
        void* pOutput,
        const void* pInput,
        ma_uint32 frameCount
    );
    static size_t FindAvailableSlot(void);
    static void MixSamplesWithConversion(
        float* output,
        const float* input,
        ma_uint32 frameCount,
        ma_uint32 inputChannels,
        ma_uint32 outputChannels,
        float volume
    );
    static void ApplyLimiter(float* buffer, ma_uint32 sampleCount);
    static uint64_t GetUniqueId(void);
    static bool EnsureInitialized(void);
    static void Cleanup(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize the audio system
    ///
    /// \return True if initialization was successful
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool Initialize(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Play an audio file
    ///
    /// \param path Path to the audio file
    /// \param id Optional identifier for the sound (auto-generated if empty)
    /// \param loop Whether to loop the audio (false by default)
    /// \param volume Volume level from 0.0 to 1.0 (default 1.0)
    /// \return Identifier of the sound instance
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::string Play(
        const std::string& path,
        const std::string& id = "",
        bool loop = false,
        float volume = 1.0f
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Stop playing a specific audio
    ///
    /// \param id Identifier of the audio to stop
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Stop(const std::string& id);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Stop all playing audio
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void StopAll();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set volume for a specific sound
    ///
    /// \param id Identifier of the sound
    /// \param volume Volume level from 0.0 to 1.0
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetVolume(const std::string& id, float volume);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load an audio file into memory cache
    ///
    /// \param path Path to the audio file
    /// \return True if loading was successful
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool Preload(const std::string& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Release audio data from cache
    ///
    /// \param path Path to the audio file to release
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void ReleaseFromCache(const std::string& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clean up audio resources
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Shutdown(void);
};

} // namespace Arc
