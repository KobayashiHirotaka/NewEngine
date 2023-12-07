#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <cassert>
#include <fstream>

struct ChunkHeader
{
    char id[4];//チャンクID
    int32_t size;//チャンクサイズ
};

struct RiffHeader
{
    ChunkHeader chunk;//"RIFF"
    char type[4];//"WAVE"
};

struct FormatChunk
{
    ChunkHeader chunk;//"fmt "
    WAVEFORMATEX fmt;//フォーマット
};

struct SoundData
{
    //波形フォーマット
    WAVEFORMATEX wfex;
    //バッファの先頭
    BYTE* pBuffer;
    //バッファのサイズ
    unsigned int bufferSize;
};

class Audio
{
public:
    static Audio* GetInstance();

    //Audio();
    //~Audio();

    void Initialize();

    SoundData SoundLoadWave(const char* filename);

    void Play(IXAudio2* xAudio2, const SoundData& soundData);

    void SoundUnload(SoundData* soundData);

    HRESULT hr;
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2 = nullptr;
    IXAudio2MasteringVoice* pMasteringVoice = nullptr;

    SoundData soundDatas[10];
};
