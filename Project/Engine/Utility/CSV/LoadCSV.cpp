/**
 * @file LoadCSV.cpp
 * @brief CSVの読み込み
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "LoadCSV.h"
std::stringstream LoadCSV(const std::string& filename)
{
	std::ifstream file;
	file.open(filename);
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	std::stringstream Commands;
	Commands << file.rdbuf();
	// ファイルを閉じる
	file.close();

	return Commands;
}