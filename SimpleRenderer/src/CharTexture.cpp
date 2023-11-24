#include "CharTexture.h"

#include <cassert>
#include <iostream>
#include <limits>

PixelData::PixelData()
	: Data(0), Depth(1000000.0f)
{}

PixelData::~PixelData()
{}

CharTexture_Accessor::CharTexture_Accessor(const CharTexture* _owner)
	: Owner(_owner), Texture(nullptr)
{}

PixelData& CharTexture_Accessor::operator[](const unsigned int& _index)
{
	assert(_index < Owner->Height && "_index out of range!");

	return *(reinterpret_cast<PixelData*>(this->Texture) + (_index * Owner->Width));
}

//////////////////////////////////////////////////

CharTexture::CharTexture(const unsigned int& _width, const unsigned int& _height)
	: Width(_width), Height(_height), Length(_width * _height), Accessor(this)
{
	Texture = new PixelData[Length];
	Clear();
}

CharTexture::~CharTexture()
{
	delete[] Texture;
}

void CharTexture::SetData(wchar_t* const _data)
{
	for (unsigned int y = 0; y < Height; ++y)
	{
		for (unsigned int x = 0; x < Width; ++x)
		{
			unsigned int index = x + y * Width;
			Texture[index].Data = _data[index];
		}
	}
}

void CharTexture::Clear()
{
	for (unsigned int i = 0; i < Length; ++i)
	{
		Texture[i].Data = 0;
		Texture[i].Depth = std::numeric_limits<float>::max();
	}
}

void CharTexture::Debug()
{
	for (unsigned int i = 0; i < Height; ++i)
	{
		for (unsigned int j = 0; j < Width; ++j)
		{
			std::cout << (*this)[j][i].Data << ", ";
		}
		std::cout << std::endl;
	}
}

CharTexture_Accessor& CharTexture::operator[](const unsigned int& _index)
{
	assert(_index < Width && "_index out of range!");

	Accessor.Texture = Texture + _index;
	return Accessor;
}
