#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <io.h>
#include <direct.h>	// visual studio only

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h" // https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h" //https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

std::string FileName(const char* _filename)
{
	const char* _s = _filename + strlen(_filename);
	while (--_s > _filename)
	{
		if (*_s == '.')
			break;
	}
	const char* _ext = _s;

	while (--_s > _filename)
	{
		if (*_s == '\\' || *_s == '/')
			break;
	}
	return std::string(_s, _ext);
}

bool ConvertImage(const char* _src, const char* _targetDir)
{
	printf("Load \"%s\"\n", _src);
	int _w, _h, _c;
	auto _img = stbi_load(_src, &_w, &_h, &_c, 0);

	if (!_img)
	{
		printf("Error: %s\n", stbi_failure_reason());
		return false;
	}

	std::string _target = _targetDir + std::string("/") + FileName(_src) + ".tga";
	printf("Save \"%s\"\n", _target.c_str());

	stbi_write_tga_with_rle = 0; // disable rle compression
	int _r = stbi_write_tga(_target.c_str(), _w, _h, _c, _img);
	stbi_image_free(_img);

	if (!_r)
	{
		printf("Error: Unable to write \"%s\"\n", _target.c_str());
		return false;
	}

	return true;
}

int main(int _argc, const char** _argv)
{
	if (_argc < 3)
	{
		printf("Usage: source destination\n");
		return -1;
	}

	_finddata_t _fd;
	auto _ffh = _findfirst(_argv[1], &_fd);
	if (_ffh < 0)
	{
		printf("source file \"%s\" not found\n", _argv[1]);
		return -1;
	}

	int _numFiles = 0;
	do
	{
		if (!ConvertImage(_fd.name, _argv[2]))
		{
			_findclose(_ffh);
			return -1;
		}
		++_numFiles;

	} while (!_findnext(_ffh, &_fd));

	printf("%d files successfully converted\n", _numFiles);
	return 0;
}