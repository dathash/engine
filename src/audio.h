// Author: Alex Hartford
// Program: Engine
// File: Audio
// Date: April 2023

#ifndef AUDIO_H
#define AUDIO_H

// The API I want:
// * Define sounds in a file
// * One audio format (preferably ogg vorbis)
// * Decode all audio offline, keep... buffers? In the stack?

// * Call GetSound(name) to get a pointer to the sound.
// * call start/stop/pause on that pointer.
// If an object has a sound, have its constructor get that pointer, and hold onto it.

// * Volume control over SFX, Music, Voice, separate.
// * Volume control over individual sounds, stored in variables file.


// NOTE: we don't need pitching, so we can pass this flag and get some speedup.

static ma_engine GlobalAudioEngine;
static ma_sound_group GlobalMusicGroup;
static ma_sound_group GlobalSfxGroup;

enum AudioType
{
    MUSIC,
    SFX,
};

struct Sound
{
    ma_sound sound;
    string name;

    Sound(const string &name_in, AudioType type_in)
    : name(name_in)
    {
        switch(type_in)
        {
            case MUSIC:
            {
        ma_sound_init_from_file(&GlobalAudioEngine, 
                                (music_path + name_in).c_str(),
                                MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, 
                                &GlobalMusicGroup, NULL, &sound);
                ma_sound_set_looping(&sound, true);
            } break;
            case SFX:
            {
        ma_sound_init_from_file(&GlobalAudioEngine, 
                                (sfx_path + name_in).c_str(),
                                MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, 
                                &GlobalSfxGroup, NULL, &sound);
            } break;
        }
    }

    ~Sound()
    {
        ma_sound_uninit(&sound);
    }

    void
    Update()
    {
    }

    void
    Pause()
    {
        ma_sound_stop(&sound);
    }

    void
    Stop()
    {
        ma_sound_stop(&sound);
        ma_sound_seek_to_pcm_frame(&sound, 0);
    }

    void
    Start()
    {
        ma_sound_start(&sound);
    }

    void
    Restart()
    {
        ma_sound_seek_to_pcm_frame(&sound, 0);
        Start();
    }
};


///////////////////////////////////////////////////////////////////////////
struct AudioBank
{
    vector<Sound *> sounds;
};

static AudioBank GlobalMusic;
static AudioBank GlobalSfx;

// Usage API
void
StartMusic(const string &name)
{
    for(Sound *sound : GlobalMusic.sounds)
        if(sound->name == name)
            sound->Start();
}

void
StopMusic(const string &name)
{
    for(Sound *sound : GlobalMusic.sounds)
        if(sound->name == name)
            sound->Stop();
}

void
PauseMusic(const string &name)
{
    for(Sound *sound : GlobalMusic.sounds)
        if(sound->name == name)
            sound->Pause();
}


void
PlaySfx(const string &name)
{
    for(Sound *sound : GlobalSfx.sounds)
    {
        if(sound->name == name)
        {
            sound->Stop();
            sound->Start();
        }
    }
}

void
StopSfx(const string &name)
{
    for(Sound *sound : GlobalSfx.sounds)
    {
        if(sound->name == name)
        {
            sound->Stop();
        }
    }
}

void
SetMusicVolume(float volume)
{
    ma_sound_group_set_volume(&(GlobalMusicGroup), volume);
}
void
SetSfxVolume(float volume)
{
    ma_sound_group_set_volume(&(GlobalSfxGroup), volume);
}

#endif
