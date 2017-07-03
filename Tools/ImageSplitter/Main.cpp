#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdint.h>

#include <io.h>
#include <direct.h>	// visual studio only

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h" // https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h" //https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

std::string FileDir(const char* _filename)
{
	const char* _s = _filename + strlen(_filename);
	while (--_s > _filename)
	{
		if (*_s == '.')
			break;
	}
	return std::string(_filename, _s);
}

int main(int _argc, const char** _argv)
{
	if (_argc < 5)
	{
		printf("Usage: source x y border\n");
		return -1;
	}

	printf("Load \"%s\"\n", _argv[1]);
	int _w, _h, _c;
	auto _img = stbi_load(_argv[1], &_w, &_h, &_c, 0);

	if (!_img)
	{
		printf("Error: %s\n", stbi_failure_reason());
		return false;
	}

	int _cellsX = 0, _cellsY = 0, _border = 0;

	sscanf(_argv[2], "%d", &_cellsX);
	sscanf(_argv[3], "%d", &_cellsY);
	sscanf(_argv[4], "%d", &_border);

	int _srcCellWidth = _w / _cellsX;
	int _srcCellHeight = _h / _cellsY;
	int _dstCellWidth = _srcCellWidth - _border * 2;
	int _dstCellHeight = _srcCellHeight - _border * 2;

	printf("Source Image %dx%d\n", _w, _h);
	printf("Result Image %dx%d\n", _dstCellWidth, _dstCellHeight);

	uint8_t* _cell = new uint8_t[_dstCellWidth * _dstCellHeight * _c];

	std::string _dir = FileDir(_argv[1]);
	mkdir(_dir.c_str());
	_dir += "/";

	int _index = 0;
	for (int cy = 0; cy < _cellsY; ++cy)
	{
		for (int cx = 0; cx < _cellsX; ++cx)
		{
			for (int y = 0; y < _dstCellHeight; ++y)
			{
				int _sy = cy * _srcCellHeight + _border + y;
				int _sx = cx * _srcCellWidth + _border;

				//printf("%d %d to %d\n", _sx, _sy, y);

				uint8_t* _src = _img + (_sy * _w + _sx) * _c;
				uint8_t* _dst = _cell + (y * _dstCellWidth * _c);
				memcpy(_dst, _src, _dstCellWidth * _c);
			}

			char _filename[32];
			sprintf(_filename, "0%d.tga", _index);
			std::string _name = _dir;
			if (_index < 10)
				_name += '0';
			_name += _filename;
			printf("Save \"%s\"\n", _name.c_str());

			stbi_write_tga_with_rle = 0; // disable rle compression
			if (!stbi_write_tga(_name.c_str(), _dstCellWidth, _dstCellHeight, _c, _cell))
			{
				printf("Error: Unable to write \"%s\"\n", _name.c_str());
				return false;
			}
			++_index;
		}
	}

	delete[] _cell;
	stbi_image_free(_img);

	printf("%d files successfully saved\n", _cellsX * _cellsY);
	return 0;
}