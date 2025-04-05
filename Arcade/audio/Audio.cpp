///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#define MINIAUDIO_IMPLEMENTATION
#include "Arcade/audio/Audio.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Arc
///////////////////////////////////////////////////////////////////////////////
namespace Arc
{

///////////////////////////////////////////////////////////////////////////////
std::unordered_map<
    std::string, std::shared_ptr<Audio::cached_audio>
> Audio::mAudioCache;

///////////////////////////////////////////////////////////////////////////////
std::unordered_map<
    std::string, std::shared_ptr<Audio::audio_stream>
> Audio::mAudioStreams;

///////////////////////////////////////////////////////////////////////////////
std::mutex Audio::mMutex;

///////////////////////////////////////////////////////////////////////////////
Audio::~Audio()
{
    StopAll();
}

///////////////////////////////////////////////////////////////////////////////
void Audio::DataCallback(
    ma_device* pDevice,
    void* pOutput,
    const void* pInput,
    ma_uint32 frameCount
)
{
    (void)pInput;
    audio_stream* stream = (audio_stream*)pDevice->pUserData;

    if (stream == nullptr || stream->cachedData == nullptr) {
        return;
    }

    ma_decoder* pDecoder = &stream->cachedData->decoder;
    ma_uint64 framesRead;

    void* tempBuffer = pOutput;
    bool useTemp = stream->volume != 1.0f;

    if (useTemp) {
        tempBuffer = malloc(frameCount * pDecoder->outputChannels *
                           ma_get_bytes_per_sample(pDecoder->outputFormat));
        if (!tempBuffer) {
            useTemp = false;
            tempBuffer = pOutput;
        }
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);

    if (useTemp && stream->volume != 1.0f) {
        if (pDecoder->outputFormat == ma_format_f32) {
            float* samples = (float*)tempBuffer;
            for (ma_uint32 i = 0; i < framesRead * pDecoder->outputChannels; i++) {
                samples[i] *= stream->volume;
            }
        } else if (pDecoder->outputFormat == ma_format_s16) {
            int16_t* samples = (int16_t*)tempBuffer;
            for (ma_uint32 i = 0; i < framesRead * pDecoder->outputChannels; i++) {
                samples[i] = (int16_t)(samples[i] * stream->volume);
            }
        }

        memcpy(pOutput, tempBuffer, framesRead * pDecoder->outputChannels *
              ma_get_bytes_per_sample(pDecoder->outputFormat));
        free(tempBuffer);
    }

    if (framesRead < frameCount && !stream->loop) {
        memset(
            (uint8_t*)pOutput + (framesRead *
                pDecoder->outputChannels *
                ma_get_bytes_per_sample(pDecoder->outputFormat)),
            0,
            (frameCount - framesRead) *
                pDecoder->outputChannels *
                ma_get_bytes_per_sample(pDecoder->outputFormat)
        );
        stream->finished = 1;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Audio::AudioThread(audio_stream* stream)
{
    ma_result result = ma_device_start(&stream->device);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&stream->device);
        ReleaseFromCache(stream->sourcePath);
        stream->finished = 1;
        return;
    }

    while (!stream->finished && !stream->stop) {
        ma_sleep(10);
    }

    ma_device_uninit(&stream->device);
    ReleaseFromCache(stream->sourcePath);

    std::lock_guard<std::mutex> lock(mMutex);
    auto it = mAudioStreams.find(stream->id);
    if (it != mAudioStreams.end() && it->second.get() == stream) {
        mAudioStreams.erase(it);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Audio::ReleaseFromCache(const std::string& path)
{
    std::lock_guard<std::mutex> lock(mMutex);
    auto cacheIt = mAudioCache.find(path);
    if (cacheIt != mAudioCache.end()) {
        cacheIt->second->refCount--;
        if (cacheIt->second->refCount <= 0) {
            ma_decoder_uninit(&cacheIt->second->decoder);
            mAudioCache.erase(path);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Audio::Play(const std::string& path, const std::string& id, bool loop)
{
    std::string soundId = id.empty() ? path + "_" +
        std::to_string(GetUniqueId()) : id;

    std::shared_ptr<audio_stream> stream = std::make_shared<audio_stream>();
    stream->finished = 0;
    stream->stop = 0;
    stream->id = soundId;
    stream->sourcePath = path;
    stream->loop = loop;

    std::shared_ptr<cached_audio> cachedData;

    {
        std::lock_guard<std::mutex> lock(mMutex);
        auto cacheIt = mAudioCache.find(path);

        if (cacheIt != mAudioCache.end()) {
            cachedData = cacheIt->second;
            cachedData->refCount++;
        } else {
            cachedData = std::make_shared<cached_audio>();
            ma_result result = ma_decoder_init_file(path.c_str(), NULL,
                &cachedData->decoder);
            if (result != MA_SUCCESS) {
                return;
            }
            cachedData->refCount = 1;
            mAudioCache[path] = cachedData;
        }
    }

    stream->cachedData = cachedData;

    stream->volume = 1.f;
    stream->deviceConfig = ma_device_config_init(ma_device_type_playback);
    stream->deviceConfig.playback.format = cachedData->decoder.outputFormat;
    stream->deviceConfig.playback.channels = cachedData->decoder.outputChannels;
    stream->deviceConfig.sampleRate = cachedData->decoder.outputSampleRate;
    stream->deviceConfig.dataCallback = Audio::DataCallback;
    stream->deviceConfig.pUserData = stream.get();

    ma_result result = ma_device_init(NULL, &stream->deviceConfig,
        &stream->device);
    if (result != MA_SUCCESS) {
        ReleaseFromCache(path);
        return;
    }

    std::lock_guard<std::mutex> lock(mMutex);

    auto existingStream = mAudioStreams.find(soundId);
    if (existingStream != mAudioStreams.end()) {
        existingStream->second->stop = 1;
    }

    mAudioStreams[soundId] = stream;

    std::thread audioThread(Audio::AudioThread, stream.get());
    audioThread.detach();
}

///////////////////////////////////////////////////////////////////////////////
void Audio::Stop(const std::string& id)
{
    std::lock_guard<std::mutex> lock(mMutex);

    auto streamIt = mAudioStreams.find(id);
    if (streamIt != mAudioStreams.end()) {
        streamIt->second->stop = 1;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Audio::StopAll()
{
    std::lock_guard<std::mutex> lock(mMutex);

    for (auto& pair : mAudioStreams) {
        pair.second->stop = 1;
    }

    ma_sleep(50);

    mAudioStreams.clear();

    for (auto& pair : mAudioCache) {
        ma_decoder_uninit(&pair.second->decoder);
    }
    mAudioCache.clear();
}

///////////////////////////////////////////////////////////////////////////////
uint64_t Audio::GetUniqueId()
{
    static std::atomic<uint64_t> counter(0);
    return counter++;
}

} // namespace Arc
