#include "Audio.h"
#include <cassert>

Audio* Audio::GetInstance()
{
	static Audio instance;
	return &instance;
}

void Audio::Initialize()
{
	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
}

uint32_t Audio::SoundLoadWave(const char* filename)
{
	//同じ音声データがないか探す
	for (SoundData& soundData : soundDatas_)
	{
		if (soundData.name == filename)
		{
			return soundData.audioHandle;
		}
	}

	audioHandle_++;

	//ファイル入力ストリームのインスタンス
	std::ifstream file;

	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);

	//ファイルオープン失敗を検出
	assert(file.is_open());

	//RIFFチャンク読み込み
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

	//formatチャンク読み込み
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
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	//Dataチャンクの波形データを読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	soundDatas_[audioHandle_].wfex = format.fmt;
	soundDatas_[audioHandle_].pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundDatas_[audioHandle_].bufferSize = data.size;
	soundDatas_[audioHandle_].name = filename;
	soundDatas_[audioHandle_].audioHandle = audioHandle_;

	return audioHandle_;
}

uint32_t Audio::SoundPlayWave(uint32_t audioHandle, bool loopFlag, float volume) 
{
	HRESULT hr;
	voiceHandle_++;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundDatas_[audioHandle].wfex);
	assert(SUCCEEDED(hr));

	Voice* voice = new Voice();
	voice->handle = audioHandle;
	voice->sourceVoice = pSourceVoice;
	sourceVoices_.insert(voice);

	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = soundDatas_[audioHandle].pBuffer;
	buffer.AudioBytes = soundDatas_[audioHandle].bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	if (loopFlag)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	hr = pSourceVoice->SubmitSourceBuffer(&buffer);
	pSourceVoice->SetVolume(volume);
	hr = pSourceVoice->Start();

	return voiceHandle_;
}

void Audio::SoundUnload(SoundData* soundData)
{
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::StopAudio(uint32_t voiceHandle) 
{
	HRESULT result;

	auto it = sourceVoices_.begin();

	while (it != sourceVoices_.end()) 
	{
		if ((*it)->handle == voiceHandle)
		{
			result = (*it)->sourceVoice->Stop();
			delete (*it);
			it = sourceVoices_.erase(it);
		}
		else {
			++it;
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

	for (int i = 0; i < soundDatas_.size(); i++)
	{
		SoundUnload(&soundDatas_[i]);
	}
}