#pragma once
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#endif
#include <string>
#include <map>
#include <vector>
#include "Color.h"
#include "Point.h"
#include "Texture.h"
#include "Camera.h"
#include "Global.h"

extern const std::string PRINTABLE_CHARACTERS;
extern const int NUM_OF_PRINTABLE_CHARACTERS;
extern const unsigned STARTING_CHARACTER_ASCII_CODE;

class TextObject;

class IFont
{
public:
	enum Alignment { LEFT, CENTER, RIGHT };

	virtual ~IFont() {};
	virtual const Point &MeasureString(const std::string &str, bool saveLength = false) = 0;
	void ClearMeasuredStrings();
	int GetNumOfMeasuredStrings() const;

	const std::string &GetPath() const;
	int GetFontSize() const;
	int GetFontOutline() const;
	int GetFontStyle() const;
protected:
	IFont(const std::string &path, int fontSize, int fontOutline = 0, int fontStyle = TTF_STYLE_NORMAL);
	virtual void LoadFont() = 0;
	virtual std::string RetrieveUniqueName() const;
    IFont(const IFont &copy); //No copy constructor since there shouldn't be any need for 2 of the same exact font
	std::map<std::string, Point> mMeasuredStrings;
	std::string mPath, mPrevMeasureString;
	Point mMeasureStringHolder;
	int mFontSize, mFontOutline, mFontStyle;
};

struct IDropShadow
{
public:
	IDropShadow();
	IDropShadow(int offsetX, int offsetY, const Color &color);
	virtual ~IDropShadow() = 0;
	const Color *DropShadowColor() const;
	int DropShadowOffsetX() const;
	int DropShadowOffsetY() const;
protected:
	Color *mShadowColor;
	int mOffsetX, mOffsetY;
private:
	IDropShadow(const IDropShadow &copy);
};

class SpriteFont : public IFont, public IDropShadow
{
public:
	/*
	USE THIS IF YOU NEED CHANGING TEXT (RENDER SPEED IS SLOWER FOR LONGER TEXT AND A SINGLE SPRITEFONT CONSUMES A LOT OF MEMORY 
	SO IF THE TEXT IS STATIC THEN JUST CREATE AN IMAGE AND USE A TEXTURE)
	Supports PNG. To use TTF/OTF as sprite fonts use the overloaded constructor
	path - location and name of Font being used (e.g. Fonts/myFont.png) 
	[NOTE: an xml file with the glyph metrics must be in the same location as the font and must also have the same name not including the extension]
	fontColor - set the color of the font
	*/ //NOT YET IMPLEMENTED
	//SpriteFont(const std::string &path, const Color &fontColor = Color(WHITE));

	/*
	USE THIS IF YOU NEED CHANGING TEXT (RENDER SPEED IS SLOWER FOR LONGER TEXT AND A SINGLE SPRITEFONT CONSUMES A LOT OF MEMORY 
	SO IF THE TEXT IS STATIC THEN JUST CREATE AN IMAGE AND USE A TEXTURE)
	Supports TTF and OTF. To use image files use the overloaded constructor
	path - location and name of Font being used (e.g. Fonts/myFont.bmp)
	fontSize - size of the font
	fontOutline - width of the outline (base will become transparent if outline is greater than 0)
	fontStyle - TTF_STYLE_NORMAL, TTF_STYLE_BOLD, TTF_STYLE_ITALIC, TTF_STYLE_UNDERLINE, TTF_STYLE_STRIKETHROUGH
	You can combine fontStyles using '|' (e.g. TTF_STYLE_BOLD | TTF_STYLE_ITALIC)
	fillColor - set the color of the fill
	outlineColor - set the color of the outline
	*/
	SpriteFont(const std::string &path, int fontSize, int fontOutline = 0, int fontStyle = TTF_STYLE_NORMAL, const Color &fillColor = Color(WHITE), const Color &outlineColor = Color(WHITE));
	/*
	USE THIS IF YOU NEED CHANGING TEXT (RENDER SPEED IS SLOWER FOR LONGER TEXT AND A SINGLE SPRITEFONT CONSUMES A LOT OF MEMORY 
	SO IF THE TEXT IS STATIC THEN JUST CREATE AN IMAGE AND USE A TEXTURE)
	Supports TTF and OTF. To use image files use the overloaded constructor
	path - location and name of Font being used (e.g. Fonts/myFont.bmp)
	fontSize - size of the font
	dropShadowOffsetX - amount to offset the drop shadow's x coordinate from the front text
	dropShadowOffsetY - amount to offset the drop shadow's y coordinate from the front text
	dropShadowColor - color of the drop shadow
	fontOutline - width of the outline (base will become transparent if outline is greater than 0)
	fontStyle - TTF_STYLE_NORMAL, TTF_STYLE_BOLD, TTF_STYLE_ITALIC, TTF_STYLE_UNDERLINE, TTF_STYLE_STRIKETHROUGH
	You can combine fontStyles using '|' (e.g. TTF_STYLE_BOLD | TTF_STYLE_ITALIC)
	fillColor - set the color of the fill
	outlineColor - set the color of the outline
	*/
	SpriteFont(const std::string &path, int fontSize, int dropShadowOffsetX, int dropShadowOffsetY, int fontOutline = 0, int fontStyle = TTF_STYLE_NORMAL, const Color &dropShadowColor = Color(BLACK), const Color &fillColor = Color(WHITE), const Color &outlineColor = Color(WHITE));
	virtual ~SpriteFont();
	/*
	text - string to draw
	dst - position of the text centered at 0,0
	camera - coordinates are manipulated depending on the position of this camera
	*/
	void DrawString(const std::string &text, const Point &dst, const Camera &camera);
	/*
	text - string to draw
	dst - position of the text centered at 0,0
	color - manipulates color of the texture (WHITE leaves the color as is)
	camera - coordinates are manipulated depending on the position of this camera
	*/
	void DrawString(const std::string &text, const Point &dst, const Color &color, const Camera &camera = globalCamera);
	/*
	text - string to draw
	dst - position of the text
	center - specifies the origin (center coordinates are relative to the upper left hand corner of the dst)
	color - manipulates color of the texture (WHITE leaves the color as is)
	camera - coordinates are manipulated depending on the position of this camera
	*/
	void DrawString(const std::string &text, const Point &dst, const Point &center, const Color &color = DEFAULT_COLOR, const Camera &camera = globalCamera);
	/*
	text - string to draw
	dst - position of the text
	center - specifies rotational axis and the origin (center coordinates are relative to the upper left hand corner of the dst)
	color - manipulates color of the texture (WHITE leaves the color as is)
	scale - size multiplier that indirectly manipulates text dimensions
	angle - rotates the text about the origin in degrees
	camera - coordinates are manipulated depending on the position of this camera
	*/
	void DrawString(const std::string &text, const Point &dst, const Point &center, const Color &color, float scale, double angle = 0, const Camera &camera = globalCamera);
	//Gives the width and height (as a Point) of the text passed in using the following font
	//MeasureString("").x - access width
	//MeasureString("").y - access height
	//saveLength - if text exists in the map, it will return the value, else text will be measured, saved, and returned
	//saving the length is good for long strings and text that's relatively static
	//use GetNumOfMeasuredStrings to get the current number of strings saved
	//use ClearMeasuredStrings to clear all the saved strings
	//the more strings saved, the less efficient the lookup will become
	virtual const Point &MeasureString(const std::string &text, bool saveLength = false);

	std::string FitText(const std::string &text, int width);
	Texture CreateTexture(const std::string &text, Alignment alignment = LEFT);

	const Color &GetFillColor() const;
	const Color &GetOutlineColor() const;
	const Texture &GetTexture() const;
	int GetLineSkip() const;
private:
    SpriteFont(const SpriteFont &copy); //No copy constructor since there shouldn't be any need for 2 of the same exact font, if passing in as argument use pointer
	virtual std::string RetrieveUniqueName();
	//x - column in which first non-transparent pixel appears
	//y - 0
	//w - last column in which a non-transparent pixel appears minus x + 1
	//h - textSurface->h
	Rect AlphaTrimmedWidth(SDL_Surface *textSurface, int i);
	virtual void LoadFont();
	std::vector<Rect> mCharRect;
	std::vector<Point> mCharGlyphDim;
	Color mFillColor, mOutlineColor;
	Texture mFont;
	SDL_Texture *mTexture;
	int mLineSkip, mHeight, mTab, *mAdvance;
};

class TextObject
{
	struct CharObject
	{
	public:
		inline CharObject &operator=(char rhs) { if (mParent->GetChar(mIndex) != rhs) mParent->SetChar(mIndex, rhs); return *this; }
		inline operator char() const { return mParent->GetChar(mIndex); }
		inline operator std::string() const { return ToString(mParent->GetChar(mIndex)); }
	private:
		friend class TextObject;
		TextObject *mParent;
		size_t mIndex;
	};
public:
	const static int CEN_RIGHT, CEN_BOT, CEN_CENTER;
	inline TextObject(SpriteFont *font, const std::string &str, const Point &dst, IFont::Alignment alignment = IFont::LEFT, const Point &center = POINT_ZERO, const Color &color = DEFAULT_COLOR,
		float scale = 1.0f, double angle = DEFAULT_ANGLE)
	{ Set(font, str, dst, alignment, center, color, scale, angle); }
	inline TextObject(const TextObject &copy) { Set(copy); }
	//camera - coordinates are manipulated depending on the position of this camera
	void Draw(const Camera &camera = globalCamera);
	/*
	font - draw TextObject using this font rather than TextObject's font
	camera - coordinates are manipulated depending on the position of this camera
	*/
	void Draw(SpriteFont *font, const Camera &camera = globalCamera);
	inline void Set(SpriteFont *font, const std::string &str, const Point &pos, IFont::Alignment alignment, const Point &center, const Color &color, float scale, double angle)
	{
		mCharObject.mParent = this;
		mFont = font;
		mString = str;
		mPos = pos;
		mAlignment = alignment;
		mColor = color;
		mScale = scale;
		mAngle = angle;
		mCen = center;
		Analyze(true);
	}
	inline bool Equals(SpriteFont *font, const std::string &str, const Point &pos, IFont::Alignment alignment, const Point &center, const Color &color, float scale, double angle) const
	{
		return mString == str
			&& mPos == pos
			&& mAlignment == alignment
			&& mFont == font
			&& mCen == center
			&& mColor == color
			&& mScale == scale
			&& mAngle == angle;
	}
	inline void Set(const TextObject &textObject) { Set(textObject.mFont, textObject.mString, textObject.mPos, textObject.mAlignment, textObject.mCen, textObject.mColor, textObject.mScale, textObject.mAngle); };
	inline bool Equals(const TextObject &textObject) const { return Equals(textObject.mFont, textObject.mString, textObject.mPos, textObject.mAlignment, textObject.mCen, textObject.mColor, textObject.mScale, textObject.mAngle); }

	inline void SetString(const std::string &str) { mString = str; Analyze(true); }
	inline void SetString(const char *str) { mString = str; Analyze(true); }
	inline void SetChar(size_t index, char ch) { mString[index] = ch; Analyze(true); }
	inline void SetFont(SpriteFont *font) { mFont = font; Analyze(true); }
	inline void SetAlignment(IFont::Alignment alignment) { if (mAlignment != alignment) { mAlignment = alignment; Analyze(false); } }
	inline void SetCenter(int nx, int ny) { mCen.Set(nx, ny); CalculateCenters(); }
	inline void OffsetCenter(int dx, int dy) { mCen.Offset(dx, dy); CalculateCenters(); }
	inline void ScaleCenter(float sx, float sy) { mCen.Scale(sx, sy); CalculateCenters(); }
	inline void ScaleCenter(float s) { mCen.Scale(s); CalculateCenters(); }
	inline void NegateCenter() { mCen.Negate(); CalculateCenters(); }
	inline void ZeroCenter() { mCen.Zero(); CalculateCenters(); }

	inline const std::string &GetString() const { return mString; }
	inline const char *GetCStr() const { return mString.c_str(); }
	inline char GetChar(size_t index) const { return mString[index]; }
	inline SpriteFont *GetFont() const { return mFont; }
	inline IFont::Alignment GetAlignment() const { return mAlignment; }
	inline const Point &GetCenter() const { return mCen; }
	inline const Point &GetDimension() const { return mDim; }

	inline Point &Position() { return mPos; }
	inline float &Scale() { return mScale; }
	inline double &Angle() { return mAngle; }

	inline operator std::string() const { return mString; }
	inline CharObject &operator[](size_t index) { mCharObject.mIndex = index; return mCharObject; }
	inline const char &operator[](size_t index) const { return mString[index]; }
	//Not your typical assignment operator, use Set() instead for assigning all members
	inline TextObject &operator=(const TextObject &rhs) { if (this != &rhs) mString = rhs.mString; return *this; }
	inline TextObject &operator=(const std::string &rhs) { if (mString != rhs) { mString = rhs; Analyze(true); } return *this; }
	inline TextObject &operator=(char ch) { mString = ch; Analyze(true); return *this; }
	inline TextObject &operator=(const char *s) { mString = s; Analyze(true); return *this; }
	inline TextObject &operator=(const std::initializer_list<char> &ilist) { mString = ilist; Analyze(true); return *this; }
	inline TextObject &operator+=(const TextObject &rhs) { return operator+=(rhs.mString); }
	inline TextObject &operator+=(const std::string &rhs) { mString += rhs; Analyze(true); return *this; }
	inline TextObject &operator+=(char ch) { mString += ch; Analyze(true); return *this; }
	inline TextObject &operator+=(const char *s) { mString += s; Analyze(true); return *this; }
	inline TextObject &operator+=(const std::initializer_list<char> &ilist) { mString += ilist; Analyze(true); return *this; }
	inline TextObject operator+(const TextObject &rhs) const { return TextObject(*this) += rhs; }
	inline TextObject operator+(const std::string &rhs) const { return TextObject(*this) += rhs; }
	inline TextObject operator+(char ch) const { return TextObject(*this) += ch; }
	inline TextObject operator+(const char *s) const { return TextObject(*this) += s; }
	inline TextObject operator+(const std::initializer_list<char> &ilist) const { return TextObject(*this) += ilist; }
	inline bool operator==(const TextObject &rhs) const { return mString == rhs.mString; }
	inline bool operator!=(const TextObject &rhs) const { return !operator==(rhs); }
	inline bool operator>(const TextObject &rhs) const { return mString > rhs.mString; }
	inline bool operator<(const TextObject &rhs) const { return mString < rhs.mString; }
	inline bool operator>=(const TextObject &rhs) const { return mString >= rhs.mString; }
	inline bool operator<=(const TextObject &rhs) const { return mString <= rhs.mString; }
protected:
	void Analyze(bool textChange);
	void CalculateOffsets();
	void CalculateCenters();
private:
	std::vector<std::string> mLines;
	std::vector<Point> mOffsets, mCenters;
	std::string mString;
	Point mPos, mCen, mDim;
	double mAngle;
	CharObject mCharObject;
	IFont::Alignment mAlignment;
	SpriteFont *mFont;
	Color mColor;
	float mScale;
	int mLinesSize;
};

namespace std
{
	template<> 
	struct hash<TextObject>
	{
		size_t operator()(const TextObject &textObject) const
		{
			std::hash<std::string> h;
			return h(textObject);
		}
	};
}