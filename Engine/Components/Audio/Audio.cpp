#include"Audio.h"

Audio* Audio::GetInstance()
{
	static Audio instance;
	return &instance;
}

void Audio::Initialize()
{
	hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2->CreateMasteringVoice(&pMasteringVoice);
	assert(SUCCEEDED(hr));
}

SoundData Audio::SoundLoadWave(const char* filename)
{
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

	//Dataチャンク読み込み
	ChunkHeader data;

	file.read((char*)&data, sizeof(data));
	if (strncmp(data.id, "bext", 4) == 0)
	{
		//JUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	//JUNKチャンクの場合
	if (strncmp(data.id, "junk", 4) == 0)
	{
		//JUNKチャンクの終わりまで進める
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

	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void Audio::SoundUnload(SoundData* soundData)
{
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::Play(IXAudio2* xAudio2, const SoundData& soundData)
{
	IXAudio2SourceVoice* pSourceVoice;
	hr = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(hr));

	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = soundData.pBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = soundData.bufferSize;
	/*buffer.LoopBegin = 0;
	buffer.LoopLength = 0;
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;*/

	pSourceVoice->SubmitSourceBuffer(&buffer);
	pSourceVoice->Start(0);
}

