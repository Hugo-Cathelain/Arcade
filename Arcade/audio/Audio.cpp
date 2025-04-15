///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#define MINIAUDIO_IMPLEMENTATION
#include "Arcade/audio/Audio.hpp"
#include <algorithm>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
// Static member initialization
///////////////////////////////////////////////////////////////////////////////
std::unordered_map<
    std::string, std::shared_ptr<Audio::AudioSource>
> Audio::mSources;
std::array<Audio::Sound, Audio::MAX_SOUNDS> Audio::mSoundPool;
size_t Audio::mActiveSounds = 0;
ma_device Audio::mDevice = {};
std::mutex Audio::mMutex;
std::atomic<bool> Audio::mInitialized(false);
std::vector<float> Audio::mMixBuffer;
ma_format Audio::mOutputFormat = ma_format_f32;
ma_uint32 Audio::mOutputChannels = 2;
ma_uint32 Audio::mSampleRate = 44100;

///////////////////////////////////////////////////////////////////////////////
bool Audio::Initialize(void)
{
    if (mInitialized) {
        return (true);
    }

    ma_device_config deviceConfig = ma_device_config_init(
        ma_device_type_playback
    );
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = 48000;
    deviceConfig.dataCallback = DataCallback;
    deviceConfig.pUserData = nullptr;
    deviceConfig.periodSizeInFrames = 1024;

    mOutputFormat = deviceConfig.playback.format;
    mOutputChannels = deviceConfig.playback.channels;
    mSampleRate = deviceConfig.sampleRate;

    ma_result result = ma_device_init(NULL, &deviceConfig, &mDevice);
    if (result != MA_SUCCESS) {
        return (false);
    }

    mMixBuffer.resize(mOutputChannels * 8192);

    result = ma_device_start(&mDevice);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&mDevice);
        return (false);
    }

    mInitialized = true;
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
void Audio::DataCallback(
    ma_device* pDevice,
    void* pOutput,
    const void* pInput,
    ma_uint32 frameCount
)
{
    (void)pDevice;
    (void)pInput;

    if (!mInitialized) {
        return;
    }

    float* outputBuffer = static_cast<float*>(pOutput);
    std::fill(
        outputBuffer,
        outputBuffer + (frameCount * mOutputChannels),
        0.0f
    );

    std::unique_lock<std::mutex> lock(mMutex, std::try_to_lock);
    if (!lock.owns_lock() || mActiveSounds == 0) {
        return;
    }

    if (mMixBuffer.size() < frameCount * mOutputChannels) {
        mMixBuffer.resize(frameCount * mOutputChannels);
    }

    std::fill(
        mMixBuffer.begin(),
        mMixBuffer.begin() + frameCount * mOutputChannels,
        0.0f
    );

    for (auto& sound : mSoundPool) {
        if (sound.status != SoundStatus::PLAYING || !sound.source) {
            continue;
        }

        ma_uint32 decoderChannels = sound.source->decoder.outputChannels;

        std::vector<float> tempBuffer(frameCount * decoderChannels * 2);

        ma_uint32 totalFramesProcessed = 0;

        while (totalFramesProcessed < frameCount) {
            ma_uint32 framesToProcess = frameCount - totalFramesProcessed;
            ma_uint64 framesRead = 0;

            ma_result result = ma_decoder_read_pcm_frames(
                &sound.source->decoder,
                tempBuffer.data(),
                framesToProcess,
                &framesRead
            );

            if (framesRead == 0 && result != MA_SUCCESS) {
                sound.status = SoundStatus::STOPPED;
                break;
            }

            if (framesRead < framesToProcess) {
                if (sound.loop) {
                    ma_decoder_seek_to_pcm_frame(&sound.source->decoder, 0);

                    if (framesRead < framesToProcess) {
                        ma_uint64 remainingFrames = 0;
                        ma_decoder_read_pcm_frames(
                            &sound.source->decoder,
                            tempBuffer.data() + (framesRead * decoderChannels),
                            framesToProcess - framesRead,
                            &remainingFrames
                        );
                        framesRead += remainingFrames;
                    }
                } else if (framesRead == 0) {
                    sound.status = SoundStatus::STOPPED;
                    break;
                }
            }

            MixSamplesWithConversion(
                mMixBuffer.data() + (totalFramesProcessed * mOutputChannels),
                tempBuffer.data(),
                framesRead,
                decoderChannels,
                mOutputChannels,
                sound.volume
            );

            totalFramesProcessed += framesRead;
            sound.position += framesRead;

            if (framesRead < framesToProcess && !sound.loop) {
                break;
            }
        }
    }

    ApplyLimiter(mMixBuffer.data(), frameCount * mOutputChannels);

    memcpy(
        pOutput,
        mMixBuffer.data(),
        frameCount * mOutputChannels * sizeof(float)
    );
}

///////////////////////////////////////////////////////////////////////////////
void Audio::MixSamplesWithConversion(
    float* output,
    const float* input,
    ma_uint32 frameCount,
    ma_uint32 inputChannels,
    ma_uint32 outputChannels,
    float volume
)
{
    for (ma_uint32 i = 0; i < frameCount; i++) {
        if (inputChannels == 1 && outputChannels == 2) {
            float sample = input[i] * volume;
            output[i*2] += sample;
            output[i*2+1] += sample;
        }
        else if (inputChannels == 2 && outputChannels == 1) {
            float left = input[i*2];
            float right = input[i*2+1];
            output[i] += ((left + right) / 2.0f) * volume;
        }
        else if (inputChannels == outputChannels) {
            for (ma_uint32 c = 0; c < outputChannels; c++) {
                output[i*outputChannels + c] +=
                    input[i*inputChannels + c] * volume;
            }
        }
        else {
            for (ma_uint32 c = 0; c < std::min(
                inputChannels, outputChannels
            ); c++) {
                output[i*outputChannels + c] +=
                    input[i*inputChannels + c] * volume;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Audio::ApplyLimiter(float* buffer, ma_uint32 sampleCount)
{
    constexpr float threshold = 0.95f;
    constexpr float ceiling = 1.0f;

    float maxAmp = 0.0f;
    for (ma_uint32 i = 0; i < sampleCount; i++) {
        float absVal = std::abs(buffer[i]);
        if (absVal > maxAmp) {
            maxAmp = absVal;
        }
    }

    if (maxAmp <= threshold) {
        return;
    }

    float gainReduction = ceiling / maxAmp;

    for (ma_uint32 i = 0; i < sampleCount; i++) {
        buffer[i] *= gainReduction;
    }
}

///////////////////////////////////////////////////////////////////////////////
std::string Audio::Play(
    const std::string& path,
    const std::string& id,
    bool loop,
    float volume
)
{
    if (!EnsureInitialized()) {
        return ("");
    }

    std::string soundId = id.empty() ? path + "_" +
        std::to_string(GetUniqueId()) : id;

    std::lock_guard<std::mutex> lock(mMutex);

    size_t slotIndex = FindAvailableSlot();
    if (slotIndex >= MAX_SOUNDS) {
        return ("");
    }

    std::shared_ptr<AudioSource> source;
    auto sourceIt = mSources.find(path);

    if (sourceIt != mSources.end()) {
        source = sourceIt->second;
        source->refCount++;
    } else {
        source = std::make_shared<AudioSource>();
        source->path = path;

        ma_decoder_config decoderConfig = ma_decoder_config_init(
            ma_format_f32, 0, 0
        );
        ma_result result = ma_decoder_init_file(
            path.c_str(), &decoderConfig, &source->decoder
        );
        if (result != MA_SUCCESS) {
            return ("");
        }
        source->refCount = 1;
        mSources[path] = source;
    }

    for (auto& sound : mSoundPool) {
        if (sound.id == soundId && sound.status == SoundStatus::PLAYING) {
            sound.status = SoundStatus::STOPPED;
        }
    }

    Sound& sound = mSoundPool[slotIndex];
    sound.id = soundId;
    sound.source = source;
    sound.position = 0;
    sound.volume = std::max(0.0f, std::min(1.0f, volume));
    sound.loop = loop;
    sound.status = SoundStatus::PLAYING;

    ma_decoder_seek_to_pcm_frame(&sound.source->decoder, 0);

    mActiveSounds++;

    return (soundId);
}

///////////////////////////////////////////////////////////////////////////////
void Audio::Stop(const std::string& id)
{
    if (!mInitialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(mMutex);

    for (auto& sound : mSoundPool) {
        if (sound.id == id && sound.status == SoundStatus::PLAYING) {
            sound.status = SoundStatus::STOPPED;
            mActiveSounds--;
            if (sound.source) {
                auto sourceIt = mSources.find(sound.source->path);
                if (sourceIt != mSources.end()) {
                    sourceIt->second->refCount--;
                    if (sourceIt->second->refCount <= 0) {
                        ma_decoder_uninit(&sourceIt->second->decoder);
                        mSources.erase(sourceIt);
                    }
                }
                sound.source = nullptr;
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Audio::StopAll(void)
{
    if (!mInitialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(mMutex);

    for (auto& sound : mSoundPool) {
        if (sound.status == SoundStatus::PLAYING) {
            sound.status = SoundStatus::STOPPED;
            if (sound.source) {
                auto sourceIt = mSources.find(sound.source->path);
                if (sourceIt != mSources.end()) {
                    sourceIt->second->refCount--;
                }
                sound.source = nullptr;
            }
        }
    }

    auto it = mSources.begin();
    while (it != mSources.end()) {
        if (it->second->refCount <= 0) {
            ma_decoder_uninit(&it->second->decoder);
            it = mSources.erase(it);
        } else {
            ++it;
        }
    }

    mActiveSounds = 0;
}

///////////////////////////////////////////////////////////////////////////////
void Audio::SetVolume(const std::string& id, float volume)
{
    if (!mInitialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(mMutex);

    for (auto& sound : mSoundPool) {
        if (sound.id == id && sound.status == SoundStatus::PLAYING) {
            sound.volume = std::max(0.0f, std::min(1.0f, volume));
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Audio::Preload(const std::string& path)
{
    if (!EnsureInitialized()) {
        return (false);
    }

    std::lock_guard<std::mutex> lock(mMutex);

    if (mSources.find(path) != mSources.end()) {
        return (true);
    }

    auto source = std::make_shared<AudioSource>();
    source->path = path;
    ma_result result = ma_decoder_init_file(
        path.c_str(), NULL, &source->decoder
    );
    if (result != MA_SUCCESS) {
        return (false);
    }

    source->refCount = 0;
    mSources[path] = source;

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
void Audio::ReleaseFromCache(const std::string& path)
{
    if (!mInitialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(mMutex);

    auto sourceIt = mSources.find(path);
    if (sourceIt != mSources.end() && sourceIt->second->refCount <= 0) {
        ma_decoder_uninit(&sourceIt->second->decoder);
        mSources.erase(sourceIt);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Audio::Shutdown(void)
{
    if (!mInitialized) {
        return;
    }

    StopAll();

    ma_device_uninit(&mDevice);
    mInitialized = false;
}

///////////////////////////////////////////////////////////////////////////////
size_t Audio::FindAvailableSlot(void)
{
    for (size_t i = 0; i < MAX_SOUNDS; ++i) {
        if (mSoundPool[i].status == SoundStatus::STOPPED) {
            return (i);
        }
    }

    if (mActiveSounds >= MAX_SOUNDS) {
        return (MAX_SOUNDS);
    }

    for (size_t i = 0; i < MAX_SOUNDS; ++i) {
        if (mSoundPool[i].source == nullptr) {
            return (i);
        }
    }

    return (MAX_SOUNDS);
}

///////////////////////////////////////////////////////////////////////////////
uint64_t Audio::GetUniqueId(void)
{
    static std::atomic<uint64_t> counter(0);
    return (counter++);
}

///////////////////////////////////////////////////////////////////////////////
bool Audio::EnsureInitialized(void)
{
    if (!mInitialized) {
        return (Initialize());
    }
    return (true);
}

} // namespace Arc
