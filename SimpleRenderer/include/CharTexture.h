#pragma once

class PixelData
{
public:
	PixelData();
	~PixelData();

	wchar_t Data;
	float Depth;
};

//////////////////////////////////////////////////

class CharTexture_Accessor
{
	friend class CharTexture;
private:
	CharTexture_Accessor(const CharTexture* _owner);
	~CharTexture_Accessor() = default;

	PixelData* Texture;
	const CharTexture* Owner;

public:
	PixelData& operator[](const unsigned int& _index);
};

//////////////////////////////////////////////////

class CharTexture
{
	friend class CharTexture_Accessor;
public:
	CharTexture(const unsigned int& _width, const unsigned int& _height);
	virtual ~CharTexture();

	inline unsigned int GetWidth() { return Width; }
	inline unsigned int GetHeight() { return Height; }

	void SetData(wchar_t* const _data);

	void Clear();
	
	void Debug();

	CharTexture_Accessor& operator[](const unsigned int& _index);

private:
	unsigned int Width, Height;
	unsigned int Length;
	PixelData* Texture;

	CharTexture_Accessor Accessor;
};

