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
	//ボイスの識別ハンドル
	uint32_t handle = 0;

	//XAudio2のソースボイス
	IXAudio2SourceVoice* sourceVoice = nullptr;
};

namespace Engine
{
	class Audio
	{
	public:
		//最大サウンドデータ数
		static const int kMaxSoundData = 256;

		/// <summary>インスタンスの取得</summary>
		static Audio* GetInstance();

		/// <summary>初期化</summary>
		void Initialize();

		/// <summary>音声読み込み(WAVE)</summary>
		uint32_t LoadSoundWave(const char* filename);

		/// <summary>音声読み込み(MP3)</summary>
		uint32_t LoadSoundMP3(const std::filesystem::path& filename);

		/// <summary>音声再生(WAVE)</summary>
		void PlaySoundWave(uint32_t audioHandle, bool roopFlag, float volume);

		/// <summary>音声読み込み(MP3)</summary>
		void PlaySoundMP3(uint32_t audioHandle, bool roopFlag, float volume);

		/// <summary>音声解放(WAVE)</summary>
		void UnloadSoundWave(SoundDataWave* soundData);

		/// <summary>音声解放(MP3)</summary>
		void UnloadSoundMP3(SoundDataMP3* soundData);

		/// <summary>再生中のサウンドがあるか</summary>
		bool IsAudioPlaying(uint32_t audioHandle);

		/// <summary>音声停止</summary>
		void StopAudio(uint32_t audioHandle);

		/// <summary>解放</summary>
		void Release();

	private:
		//シングルトン
		Audio() = default;
		~Audio() = default;
		Audio(const Audio&) = delete;
		const Audio& operator=(const Audio&) = delete;

	private:
		//XAudio2
		Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;

		//マスターボイス
		IXAudio2MasteringVoice* masterVoice_ = nullptr;

		//WAVEデータの配列
		std::array<SoundDataWave, kMaxSoundData> soundDatasWav_{};

		//MP3データの配列
		std::array<SoundDataMP3, kMaxSoundData> soundDatas_{};

		//ソースボイス
		std::set<Voice*> sourceVoices_{};

		//オーディオハンドル
		uint32_t audioHandle_ = 0;

		//ボイスハンドル
		uint32_t voiceHandle_ = 0;
	};
}
