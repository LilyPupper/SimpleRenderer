#pragma once

class CharTexture_Accessor
{
	friend class CharTexture;
private:
	CharTexture_Accessor(const CharTexture* _owner);
	~CharTexture_Accessor() = default;

	int* m_Texture;
	const CharTexture* m_Owner;

public:
	int& operator[](const unsigned int& _index);
};

//////////////////////////////////////////////////

class CharTexture
{
	friend class CharTexture_Accessor;
public:
	CharTexture(const unsigned int& _width, const unsigned int& _height);
	virtual ~CharTexture();

	inline unsigned int GetWidth() { return m_Width; }
	inline unsigned int GetHeight() { return m_Height; }

	void Clear();
	
	void Debug();

	CharTexture_Accessor& operator[](const unsigned int& _index);

private:
	unsigned int m_Width, m_Height;
	unsigned int m_Length;
	int* m_Texture;

	CharTexture_Accessor m_Accessor;
};

