#ifndef AUDIO
#define AUDIO

#include <utils/misc.h>
#include <string.h>

struct sound_t
{
    int16* data;
    int n_channels;
    int n_samples;
};

// struct sound_parameters
// {
//     union
//     {
//         struct
//         {
//             real volume;
//             real balance;
//         };
//         real vars[2];
//     };
// };

// struct sound_instance
// {
//     sound_t* sound;
//     sound_parameters params;
//     sound_parameters target_params;

//     uint samples_played;
//     uint64 start_sample;
// };

struct audio_context {
    IXAudio2* xaudio2;
    // int16* buffer;
    // uint buffer_length;
    // uint sample_rate;
    IXAudio2SourceVoice** source_voices;
    int n_source_voices;
    XAUDIO2_SEND_DESCRIPTOR sfx_send;
    XAUDIO2_VOICE_SENDS sends_list;
    IXAudio2SubmixVoice* effects_voice;
    IXAudio2SourceVoice* music_voice;
};

audio_context audio;

WAVEFORMATEX wft = {
    WAVE_FORMAT_PCM, //wFormatTag
    2,               //nChannels
    44100,           //nSamplesPerSec
    2*88200,         //nAvgBytesPerSec = nSamplesPerSec*nBlockAlign = nSamplesPerSec*nChannels*wBitsPerSample/8
    2*2,             //nBlockAlign = nChannels*wBitsPerSample/8
    16,              //wBitsPerSample
    0,               //cbSize
};

void init_audio_context()
{ //create sound device
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(hr == S_OK);

    IXAudio2* xaudio2 = 0;
    hr = XAudio2Create(&xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(hr == S_OK);

    IXAudio2MasteringVoice* master_voice = 0;
    hr = xaudio2->CreateMasteringVoice(&master_voice, 2, 44100, 0, 0, 0, AudioCategory_GameEffects);
    assert(hr == S_OK);

    audio = (audio_context){};
    audio.xaudio2 = xaudio2;
    audio.n_source_voices = 32;
    audio.source_voices = (IXAudio2SourceVoice**) dynamic_alloc(sizeof(audio.source_voices[0])*audio.n_source_voices);

    hr = xaudio2->CreateSubmixVoice(&audio.effects_voice, 1, 44100, 0, 0, 0, 0);
    assert(hr == S_OK);

    audio.sfx_send = {0, audio.effects_voice};
    audio.sends_list = {1, &audio.sfx_send};

    for(int i = 0; i < audio.n_source_voices; i++)
    {
        hr = xaudio2->CreateSourceVoice(audio.source_voices+i, &wft, XAUDIO2_VOICE_NOPITCH, 1.0, 0 /*callback*/, &audio.sends_list, 0);
        assert(hr == S_OK);
    }

    hr = xaudio2->CreateSourceVoice(&audio.music_voice, &wft, XAUDIO2_VOICE_NOPITCH, 1.0, 0 /*callback*/, 0, 0);
    assert(hr == S_OK);

    hr = xaudio2->StartEngine();
    assert(hr == S_OK);

    // XAUDIO2_BUFFER audio_buffer = {
    //     .Flags = 0,
    //     .AudioBytes = buffer_size,
    //     .pAudioData = (byte*) audio.buffer,
    //     .PlayBegin = 0,
    //     .PlayLength = audio.buffer_length,
    //     .LoopBegin = 0,
    //     .LoopLength = 0,
    //     .LoopCount = XAUDIO2_LOOP_INFINITE,
    //     .pContext = 0
    // };

    // hr = source_voice->SubmitSourceBuffer(&audio_buffer,0);
    // assert(hr == S_OK);
    // hr = source_voice->Start();
    // assert(hr == S_OK);
}

sound_t load_ogg(char* filename)
{
    int n_channels = 0;
    int sample_rate = 0;
    int16* data = 0;
    int n_samples = stb_vorbis_decode_filename(filename, &n_channels, &sample_rate, &data);

    assert(n_samples >= 0, "could not load ", filename, "\n");

    //TODO: resample if sample rate does not match

    return {
        .data = data,
        .n_channels = n_channels,
        .n_samples = n_samples,
    };
}

void play_sound(sound_t* sound, real volume)
{
    HRESULT hr;
    IXAudio2SourceVoice* source_voice = 0;
    int i = 0;
    while(source_voice == 0)
    {
        for(; i < audio.n_source_voices; i++)
        {
            XAUDIO2_VOICE_STATE voice_state;
            audio.source_voices[i]->GetState(&voice_state);
            if(voice_state.BuffersQueued == 0)
            {
                source_voice = audio.source_voices[i];
                break;
            }
        }
        if(source_voice == 0)
        {
            int n_source_voices = 2*audio.n_source_voices;
            audio.source_voices = (IXAudio2SourceVoice**) dynamic_realloc(audio.source_voices, n_source_voices*sizeof(audio.source_voices[0]));

            for(int i = audio.n_source_voices; i < n_source_voices; i++)
            {
                hr = audio.xaudio2->CreateSourceVoice(audio.source_voices+i, &wft, XAUDIO2_VOICE_NOPITCH, 1.0, 0 /*callback*/, &audio.sends_list, 0);
                assert(hr == S_OK);
            }
            audio.n_source_voices = n_source_voices;
        }
    }

    XAUDIO2_BUFFER audio_buffer = {
        .Flags = 0,
        .AudioBytes = sound->n_samples*sound->n_channels*sizeof(int16),
        .pAudioData = (byte*) sound->data,
        .PlayBegin = 0,
        .PlayLength = sound->n_samples,
        .LoopBegin = 0,
        .LoopLength = 0,
        .LoopCount = 0,
        .pContext = source_voice,
    };

    hr = source_voice->SubmitSourceBuffer(&audio_buffer,0);
    assert(hr == S_OK);
    hr = source_voice->Start();
    source_voice->SetVolume(volume);
    assert(hr == S_OK);
}

void play_music(sound_t* sound)
{
    HRESULT hr;
    IXAudio2SourceVoice* source_voice = audio.music_voice;

    XAUDIO2_BUFFER audio_buffer = {
        .Flags = 0,
        .AudioBytes = sound->n_samples*sound->n_channels*sizeof(int16),
        .pAudioData = (byte*) sound->data,
        .PlayBegin = 0,
        .PlayLength = sound->n_samples,
        .LoopBegin = 0,
        .LoopLength = 0,
        .LoopCount = XAUDIO2_LOOP_INFINITE,
        .pContext = source_voice,
    };

    hr = source_voice->SubmitSourceBuffer(&audio_buffer,0);
    assert(hr == S_OK);
    hr = source_voice->Start();
    assert(hr == S_OK);
}

sound_t explosion_sound = {};
sound_t explosion2_sound = {};
sound_t tentacle_hit_sound = {};
sound_t freeze_sound = {};
sound_t heal_sound = {};
sound_t menu_click_sound = {};
sound_t key_sound = {};

void load_sounds()
{
    explosion_sound = load_ogg("data/sounds/explosion.ogg");
    explosion2_sound = load_ogg("data/sounds/explosion2.ogg");
    tentacle_hit_sound = load_ogg("data/sounds/tentacle_hit.ogg");
    freeze_sound = load_ogg("data/sounds/freeze.ogg");
    heal_sound = load_ogg("data/sounds/heal.ogg");
    menu_click_sound = load_ogg("data/sounds/menu_click.ogg");
    key_sound = load_ogg("data/sounds/key.ogg");
}

#endif //AUDIO
