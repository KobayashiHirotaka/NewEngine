#pragma once
#include <array>
#include <set>
#include <fstream>
#include <wrl.h>
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")

class Audio 
{
public:
	static const int kMaxSoundData = 256;

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
		ChunkHeader chunk;//"fmt"
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
		//名前
		std::string name;
		//オーディオハンドル
		uint32_t audioHandle;
	};

	struct Voice 
	{
		uint32_t handle = 0;
		IXAudio2SourceVoice* sourceVoice = nullptr;
	};

	static Audio* GetInstance();

	void Initialize();

	uint32_t SoundLoadWave(const char* filename);

	uint32_t SoundPlayWave(uint32_t audioHandle, bool loopFlag, float volume);

	void SoundUnload(SoundData* soundData);

	void StopAudio(uint32_t voiceHandle);

	void Release();

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator = (const Audio&) = delete;

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;

	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	std::array<SoundData, kMaxSoundData> soundDatas_{};

	std::set<Voice*> sourceVoices_{};

	uint32_t audioHandle_ = -1;
	uint32_t voiceHandle_ = -1;
};