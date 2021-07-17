#pragma once
#include <fstream>
#include <istream>
#include <vector>
#include "../Globals/stdafx.h"

class FileReaderWriter
{
public:
	static size_t ReadFileBytes(WCHAR* file, char*& outBytes);
	static size_t ReadFileBytes(char const* file, char*& outBytes);
};

