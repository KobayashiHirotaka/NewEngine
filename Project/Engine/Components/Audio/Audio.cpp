/**
 * @file Audio.cpp
 * @brief 音声データの読み込み、再生、停止などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Audio.h"
#include <cassert>

Audio* Audio::GetInstance()
{
	static Audio sInstance;
	return &sInstance;
}

void Audio::Initialize()
{
	HRESULT hr;

	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
	hr = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(hr));

	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
}

uint32_t Audio::LoadSoundWave(const char* filename)
{
	//同じ音声データがないか探す
	for (SoundDataWave& soundData : soundDatasWav_)
	{
		if (soundData.name == filename)
		{
			return soundData.audioHandle;
		}
	}

	audioHandle_++;

	// ファイル入力ストリームのインスタンス
	std::ifstream file;

	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);

	// ファイルオープン失敗を検出
	assert(file.is_open());

	// RIFFチャンク読み込み
	RiffHeader riff;

	//チャンクがRIFFかチェック
	file.read((char*)&riff, sizeof(riff));
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}

	//ファイルタイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	//formatチャンクの読み込み
	FormatChunk format = {};

	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
	{
		assert(0);
	}

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	//JUNKチャンクの場合
	if (strncmp(data.id, "JUNK", 4) == 0)
	{
		//JUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	//LISTチャンクの場合
	if (strncmp(data.id, "LIST", 4) == 0)
	{
		//LISTチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	//Dataチャンクの波形データの読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	soundDatasWav_[audioHandle_].wfex = format.fmt;
	soundDatasWav_[audioHandle_].pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundDatasWav_[audioHandle_].bufferSize = data.size;
	soundDatasWav_[audioHandle_].name = filename;
	soundDatasWav_[audioHandle_].audioHandle = audioHandle_;

	return audioHandle_;
}

uint32_t Audio::LoadSoundMP3(const std::filesystem::path& filename)
{
	//同じ音声データがないか探す
	for (SoundDataMP3& soundData : soundDatas_)
	{
		if (soundData.name == filename)
		{
			return soundData.audioHandle;
		}
	}

	audioHandle_++;

	IMFSourceReader* pMFSourceReader = nullptr;
	MFCreateSourceReaderFromURL(filename.c_str(), NULL, &pMFSourceReader);

	IMFMediaType* pMFMediaType = nullptr;
	MFCreateMediaType(&pMFMediaType);
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

	pMFSourceReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pMFMediaType);

	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pMFMediaType);

	WAVEFORMATEX* waveFormat = nullptr;
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);

	std::vector<BYTE> mediaData;
	while (true)
	{
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags = 0;
		pMFSourceReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer = nullptr;
		DWORD cbCurrentLength = 0;
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

		pMFMediaBuffer->Unlock();
		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	soundDatas_[audioHandle_].wfex = *waveFormat;
	soundDatas_[audioHandle_].pBuffer = mediaData;
	soundDatas_[audioHandle_].bufferSize = mediaData.size();
	soundDatas_[audioHandle_].name = filename.string();
	soundDatas_[audioHandle_].audioHandle = audioHandle_;

	CoTaskMemFree(waveFormat);
	pMFMediaType->Release();
	pMFSourceReader->Release();

	return audioHandle_;
}

void Audio::PlaySoundWave(uint32_t audioHandle, bool roopFlag, float volume)
{
	HRESULT hr;
	voiceHandle_++;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundDatasWav_[audioHandle].wfex);
	assert(SUCCEEDED(hr));

	Voice* voice = new Voice();
	voice->handle = voiceHandle_;
	voice->sourceVoice = pSourceVoice;
	sourceVoices_.insert(voice);

	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = soundDatasWav_[audioHandle].pBuffer;
	buffer.AudioBytes = soundDatasWav_[audioHandle].bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	if (roopFlag)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	hr = pSourceVoice->SubmitSourceBuffer(&buffer);
	pSourceVoice->SetVolume(volume);
	hr = pSourceVoice->Start();
}

void Audio::PlaySoundMP3(uint32_t audioHandle, bool roopFlag, float volume)
{
	HRESULT hr;
	voiceHandle_++;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundDatas_[audioHandle].wfex);
	assert(SUCCEEDED(hr));

	Voice* voice = new Voice();
	voice->handle = voiceHandle_;
	voice->sourceVoice = pSourceVoice;
	sourceVoices_.insert(voice);

	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = soundDatas_[audioHandle].pBuffer.data();
	buffer.AudioBytes = UINT(soundDatas_[audioHandle].bufferSize);
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	if (roopFlag)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	hr = pSourceVoice->SubmitSourceBuffer(&buffer);
	pSourceVoice->SetVolume(volume);
	hr = pSourceVoice->Start();
}

void Audio::UnloadSoundWave(SoundDataWave* soundData)
{
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::UnloadSoundMP3(SoundDataMP3* soundData)
{
	soundData->pBuffer.clear();
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

bool Audio::IsAudioPlaying(uint32_t audioHandle) 
{
	for (const Voice* voice : sourceVoices_)
	{
		if (voice->handle == audioHandle)
		{
			return true;
		}
	}
	return false; 
}

void Audio::StopAudio(uint32_t audioHandle)
{
	HRESULT hr;

	for (const Voice* voice : sourceVoices_)
	{
		if (voice->handle == audioHandle)
		{
			hr = voice->sourceVoice->Stop();
		}
	}
}

void Audio::Release()
{
	for (const Voice* voice : sourceVoices_)
	{
		if (voice->sourceVoice != nullptr)
		{
			voice->sourceVoice->DestroyVoice();
		}
		delete voice;
	}

	xAudio2_.Reset();

	for (int i = 0; i < soundDatasWav_.size(); i++)
	{
		UnloadSoundWave(&soundDatasWav_[i]);
	}

	for (int i = 0; i < soundDatas_.size(); i++)
	{
		UnloadSoundMP3(&soundDatas_[i]);
	}

	MFShutdown();
	CoUninitialize();
}