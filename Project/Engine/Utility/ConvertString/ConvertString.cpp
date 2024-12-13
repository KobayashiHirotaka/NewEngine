/**
 * @file ConvertString.cpp
 * @brief ログ出力と文字列の変換
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "ConvertString.h"

void Log(const std::string& message)
{
	//ログメッセージを標準出力に表示
	OutputDebugStringA(message.c_str());
}

std::wstring ConvertString(const std::string& str)
{
	//空の文字列の場合
	if (str.empty())
	{
		return std::wstring();
	}

	//変換に必要なサイズを取得
	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);

	//変換できない場合
	if (sizeNeeded == 0)
	{
		return std::wstring();
	}

	//std::wstringに変換結果を格納
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str)
{
	//空の文字列の場合
	if (str.empty())
	{
		return std::string();
	}

	//変換に必要なサイズを取得
	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);

	//変換できない場合
	if (sizeNeeded == 0)
	{
		return std::string();
	}

	//std::stringに変換結果を格納
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}