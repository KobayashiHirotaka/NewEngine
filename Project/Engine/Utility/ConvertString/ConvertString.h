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

void Log(const std::string& message);

std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);
