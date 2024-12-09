/**
 * @file Audio.h
 * @brief 音声データの読み込み、再生、停止などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include <xaudio2.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <fstream>
#include <wrl.h>
#include <array>
#include <set>
#include <vector>
#include <filesystem>

#pragma comment (lib,"Mf.lib")
#pragma comment (lib,"mfplat.lib")
#pragma comment (lib,"Mfreadwrite.lib")
#pragma comment (lib,"mfuuid.lib")
#pragma comment (lib,"xaudio2.lib")

struct ChunkHeader
{
	char id[4];// チャンクID
	int32_t size;// チャンクサイズ
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

struct SoundDataWave
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

struct SoundDataMP3
{
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭
	std::vector<BYTE> pBuffer;
	//バッファのサイズ
	size_t bufferSize;
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


class Audio
{
public:
	static const int kMaxSoundData = 256;

	static Audio* GetInstance();

	void Initialize();

	uint32_t SoundLoadWave(const char* filename);

	uint32_t SoundLoadMP3(const std::filesystem::path& filename);

	void SoundPlayWave(uint32_t audioHandle, bool roopFlag, float volume);

	void SoundPlayMP3(uint32_t audioHandle, bool roopFlag, float volume);

	void SoundUnloadWave(SoundDataWave* soundData);

	void SoundUnloadMP3(SoundDataMP3* soundData);

	bool IsAudioPlaying(uint32_t audioHandle);

	void StopAudio(uint32_t audioHandle);

	void Release();

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator=(const Audio&) = delete;

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;

	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	std::array<SoundDataWave, kMaxSoundData> soundDatasWav_{};

	std::array<SoundDataMP3, kMaxSoundData> soundDatas_{};

	std::set<Voice*> sourceVoices_{};

	uint32_t audioHandle_ = 0;
	uint32_t voiceHandle_ = 0;
};