/**
 * @file ConvertString.h
 * @brief ログ出力と文字列の変換
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include <string>
#include <format>
#include <Windows.h>

/// <summary>ログメッセージを標準出力に表示</summary>
void Log(const std::string& message);

/// <summary>文字列をstd::wstringに変換</summary>
std::wstring ConvertString(const std::string& str);

/// <summary>std::wstringを文字列に変換</summary>
std::string ConvertString(const std::wstring& str);
