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
	: m_Owner(_owner), m_Texture(nullptr)
{}

PixelData& CharTexture_Accessor::operator[](const unsigned int& _index)
{
	assert(_index < m_Owner->m_Height && "_index out of range!");

	return *(reinterpret_cast<PixelData*>(this->m_Texture) + (_index * m_Owner->m_Width));
}

//////////////////////////////////////////////////

CharTexture::CharTexture(const unsigned int& _width, const unsigned int& _height)
	: m_Width(_width), m_Height(_height), m_Length(_width * _height), m_Accessor(this)
{
	m_Texture = new PixelData[m_Length];
	Clear();
}

CharTexture::~CharTexture()
{
	delete[] m_Texture;
}

void CharTexture::SetData(wchar_t* const _data)
{
	for (unsigned int y = 0; y < m_Height; ++y)
	{
		for (unsigned int x = 0; x < m_Width; ++x)
		{
			unsigned int index = x + y * m_Width;
			m_Texture[index].Data = _data[index];
		}
	}
}

void CharTexture::Clear()
{
	for (unsigned int i = 0; i < m_Length; ++i)
	{
		m_Texture[i].Data = 0;
		m_Texture[i].Depth = std::numeric_limits<float>::max();
	}
}

void CharTexture::Debug()
{
	for (unsigned int i = 0; i < m_Height; ++i)
	{
		for (unsigned int j = 0; j < m_Width; ++j)
		{
			std::cout << (*this)[j][i].Data << ", ";
		}
		std::cout << std::endl;
	}
}

CharTexture_Accessor& CharTexture::operator[](const unsigned int& _index)
{
	assert(_index < m_Width && "_index out of range!");

	m_Accessor.m_Texture = m_Texture + _index;
	return m_Accessor;
}
