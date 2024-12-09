/**
 * @file LoadCSV.h
 * @brief CSVの読み込み
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include<string>
#include<sstream>
#include<fstream>
#include<cassert>

std::stringstream LoadCSV(const std::string& filename);