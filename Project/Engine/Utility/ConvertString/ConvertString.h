#pragma once
#include <string>
#include <format>
#include <Windows.h>

void Log(const std::string& message);

std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);
