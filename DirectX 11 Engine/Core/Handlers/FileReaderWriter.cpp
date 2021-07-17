#include "FileReaderWriter.h"
#include <comdef.h>

size_t FileReaderWriter::ReadFileBytes(char const* filename, char*& outBytes)
{
	FILE* file;
	fopen_s(&file, filename, "rb");
	long len = 0;

	if (file != 0)
	{
		fseek(file, 0, SEEK_END);
		len = ftell(file);
		outBytes = (char*)malloc(len);
		fseek(file, 0, SEEK_SET);
		fread(outBytes, 1, len, file);
		fclose(file);
	}
	
	return static_cast<size_t>(len);
}

size_t FileReaderWriter::ReadFileBytes(WCHAR* file, char*& outBytes)
{
	_bstr_t b(file);

	char const* filename = b;

	return ReadFileBytes(filename, outBytes);
}
