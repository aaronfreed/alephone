#ifndef __AUDIO_PLAYER_H
#define __AUDIO_PLAYER_H

#include <AL/al.h>
#include <AL/alext.h>

#include "Decoder.h"
#include <atomic>
#include <algorithm>
#include <unordered_map>
#include <boost/lockfree/spsc_queue.hpp>

template <typename T>
struct AtomicStructure {
private:
    static constexpr int queue_size = 5;
    boost::lockfree::spsc_queue<T, boost::lockfree::capacity<queue_size>> shared_queue;
    T structure;
public:
    AtomicStructure& operator= (const T& structure) {
        this->structure = structure;
        return *this;
    }

    const T& Get() const { return structure; }

    void Store(const T& value) {
        shared_queue.push(value);
    }

    void Set(const T& value) {
        structure = value;
    }

    bool Consume(T& returnValue) {
        return shared_queue.pop(returnValue);
    }

    void Update() {
        T returnValue[queue_size];
        auto size = shared_queue.pop(returnValue, queue_size);
        if (size) structure = returnValue[size - 1];
    }
};

static constexpr int num_buffers = 4;
static constexpr int buffer_samples = 8192;

class AudioPlayer {
private:
   
    typedef std::unordered_map<ALuint, bool> AudioPlayerBuffers; //<buffer id, is queued for processing>

    struct AudioSource {
        ALuint source_id = 0; //Source used by this player
        AudioPlayerBuffers buffers;
    };

    virtual bool Update() { return true; };
    bool Play();
    void ResetSource();
    std::unique_ptr<AudioSource> RetrieveSource();
    bool AssignSource();
    virtual bool SetUpALSourceIdle() const; //Update of the source parameters (AL), done everytime the player is processed in the queue
    virtual bool SetUpALSourceInit() const; //Init of the source parameters (AL), done when the source is assigned to the player
    int GetCorrespondingFormat(bool stereo, bool isSixteenBit) const;

    friend class OpenALManager;
public:
    void Stop();
    bool IsActive() const { return is_active; }
    void SetVolume(float volume) { this->volume = volume; }
    void AskRewind() { rewind_state = true; }
    virtual short GetIdentifier() const { return NONE; }
    virtual short GetSourceIdentifier() const { return NONE; }
    void SetFilterable(bool filterable) { this->filterable = filterable; }
    virtual float GetPriority() const = 0;
protected:
    AudioPlayer(int rate, bool stereo, bool sixteen_bit);
    void UnqueueBuffers();
    virtual void FillBuffers();
    virtual int GetNextData(uint8* data, int length) = 0;
    std::atomic_bool rewind_state = { false };
    std::atomic_bool filterable = { true };
    std::atomic_bool is_active = { true };
    std::atomic<float> volume = { 0 };
    int rate = 0;
    ALenum format = 0; //Mono 8-16 or stereo 8-16
    std::unique_ptr<AudioSource> audio_source;
    virtual void Rewind();
};

#endif