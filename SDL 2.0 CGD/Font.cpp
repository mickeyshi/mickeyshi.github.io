#include "Font.h"
#include "Texture.h"
#include "Point.h"
#include "Conversion.h"
#include "Global.h"
#if _MSC_VER >= 1800
#include <algorithm>
#endif //VC++ 2013 Compiler (std::min and std::max)

#define FONTSIZE "##SIZE:"
#define FONTSTYLE "##STYLE:"
#define FONTOUTLINE "##OUTLINE:"
#define FONTSHADOW "##SHADOW"
const std::string PRINTABLE_CHARACTERS = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
const int NUM_OF_PRINTABLE_CHARACTERS = PRINTABLE_CHARACTERS.size();
const unsigned STARTING_CHARACTER_ASCII_CODE = (unsigned char)PRINTABLE_CHARACTERS[0];

/////////////
/// IFont ///
/////////////

IFont::IFont(const std::string &path, int fontSize, int fontOutline, int fontStyle)
	: mPath(path)
{
	mFontStyle = fontStyle;
	mFontSize = fontSize;
	mFontOutline = fontOutline;
}

void IFont::ClearMeasuredStrings()
{
	mMeasuredStrings.clear();
}
int IFont::GetNumOfMeasuredStrings() const
{
	return mMeasuredStrings.size();
}

const std::string &IFont::GetPath() const { return mPath; }
int IFont::GetFontStyle() const { return mFontStyle; }
int IFont::GetFontSize() const { return mFontSize; }
int IFont::GetFontOutline() const { return mFontOutline; }
std::string IFont::RetrieveUniqueName() const {
	return mPath +
		FONTSIZE + ToString(mFontSize) +
		FONTSTYLE + ToString(mFontStyle) +
		FONTOUTLINE + ToString(mFontOutline);
}

///////////////////
/// IDropShadow ///
///////////////////

IDropShadow::IDropShadow() { mShadowColor = NULL; mOffsetX = 0; mOffsetY = 0; }
IDropShadow::IDropShadow(int offsetX, int offsetY, const Color &color) { mOffsetX = offsetX; mOffsetY = offsetY; mShadowColor = new Color(color); }
IDropShadow::~IDropShadow() { delete mShadowColor; }
const Color *IDropShadow::DropShadowColor() const { return mShadowColor; }
int IDropShadow::DropShadowOffsetX() const { return mOffsetX; }
int IDropShadow::DropShadowOffsetY() const { return mOffsetY; }

//////////////////
/// SpriteFont ///
//////////////////

/*SpriteFont::SpriteFont(const std::string &path, const Color &fontColor)
	: IFont(path, 0, 0, TTF_STYLE_NORMAL), mFont(mPath)
{
	LoadFont();
}*/

SpriteFont::SpriteFont(const std::string &path, int fontSize, int fontOutline, int fontStyle, const Color &fillColor, const Color &outlineColor)
	: IFont(path, fontSize, fontOutline, fontStyle), mFillColor(fillColor), mOutlineColor(outlineColor)
{
	LoadFont();
}

SpriteFont::SpriteFont(const std::string &path, int fontSize, int dropShadowOffsetX, int dropShadowOffsetY, int fontOutline, int fontStyle, const Color &dropShadowColor, const Color &fillColor, const Color &outlineColor)
	: IFont(path, fontSize, fontOutline, fontStyle), IDropShadow(dropShadowOffsetX, dropShadowOffsetY, dropShadowColor), mFillColor(fillColor), mOutlineColor(outlineColor)
{
	LoadFont();
}

SpriteFont::~SpriteFont()
{
}

std::string SpriteFont::RetrieveUniqueName() { return IFont::RetrieveUniqueName() + mFillColor.ToString() + ((mFontOutline != 0) ? mOutlineColor.ToString() : "") + ((mShadowColor != NULL) ? FONTSHADOW + ToString(mOffsetX) + ToString(mOffsetY) + mShadowColor->ToString() : ""); }

Rect SpriteFont::AlphaTrimmedWidth(SDL_Surface *textSurface, int i)
{
	int *pixels = (int*)textSurface->pixels, minColumn = 0, maxColumn = textSurface->w;
	//#pragma omp parallel sections num_threads(2)
	{
		for (int column = 0; column < maxColumn; column++)
		{
			bool brk = false;
			for (int row = 0; row < textSurface->h; row++)
			{
				char buf[9];
				sprintf(buf, "%08x", pixels[row * maxColumn + column]);
				if (!(buf[0] == '0' && buf[1] == '0')) //non-transparent
				{
					minColumn = column;
					brk = true;
					break;
				}
			}
			if (brk)
				break;
		}
		//#pragma omp section
		{
			for (int column = maxColumn - 1; column >= 0; column--)
			{
				bool brk = false;
				for (int row = 0; row < textSurface->h; row++)
				{
					char buf[9];
					sprintf(buf, "%08x", pixels[row * maxColumn + column]);
					if (!(buf[0] == '0' && buf[1] == '0')) //non-transparent
					{
						maxColumn = column + 1;
						brk = true;
						break;
					}
				}
				if (brk)
					break;
			}
		}
	}
	if (minColumn >= maxColumn)
		Error("Font: " + mPath + "\tCharacter: '" + PRINTABLE_CHARACTERS[i] + "'\n\tminColumn >= maxColumn ==> " + ToString(minColumn) + " >= " + ToString(maxColumn));
	int tsWidth = maxColumn - minColumn;
	if (tsWidth <= 0)
		tsWidth = textSurface->w;
	return{ minColumn, 0, tsWidth, textSurface->h };
}

void SpriteFont::LoadFont()
{
	if (mPath.find(".ttf") != std::string::npos || mPath.find(".TTF") != std::string::npos || mPath.find(".otf") != std::string::npos || mPath.find(".OTF") != std::string::npos)
	{
		TTF_Font *font = TTF_OpenFont(mPath.c_str(), mFontSize);
		if (font == NULL)
		{
			Error("Could not load font: " + mPath + "\nCheck the path!");
			return;
		}
		TTF_SetFontStyle(font, mFontStyle);
		TTF_SetFontOutline(font, mFontOutline);
		TTF_SetFontKerning(font, 0);
		#pragma region Get Glyph Metrics
		mLineSkip = TTF_FontLineSkip(font);
		mHeight = TTF_FontHeight(font); //temporary holding
		mAdvance = new int[NUM_OF_PRINTABLE_CHARACTERS];
		int totalAdvancement = 0, maxWidth = 0, numRows = 5, row = RoundUp(NUM_OF_PRINTABLE_CHARACTERS / static_cast<double>(numRows));
		for (int i = 0; i < NUM_OF_PRINTABLE_CHARACTERS; i++)
		{
			mCharRect.push_back(Rect());
			mCharGlyphDim.push_back(Point());
			int xMax, xMin, yMax, yMin;
			TTF_GlyphMetrics(font, PRINTABLE_CHARACTERS[i], &xMin, &xMax, &yMin, &yMax, &mAdvance[i]);
			if (i % row == 0)
			{
				if (totalAdvancement > maxWidth)
					maxWidth = totalAdvancement;
				totalAdvancement = 0;
			}
			mCharRect[i].w = Round(mAdvance[i] + mFontOutline * 4 + mAdvance[0] / 2.0f + mOffsetX + 4 * mFontStyle);
			mCharRect[i].h = Round(mHeight + mFontOutline * 5 + mLineSkip / 8.0f + mOffsetY);
			mCharRect[i].x = totalAdvancement;
			mCharRect[i].y = i / row * mCharRect[i].h;
			mCharGlyphDim[i].Set(xMax - xMin, yMax - yMin);
			totalAdvancement += mCharRect[i].w;
		}
		mTab = mAdvance[0] * 8;
		mHeight = mCharRect[0].h * numRows;
		#pragma endregion
		if (R_TextureAlreadyLoaded(RetrieveUniqueName(), NULL, NULL))
		{
			mFont.SetTexture(RetrieveUniqueName(), NULL);
			TTF_CloseFont(font);
		}
		else
		{
			#pragma region Create Sprite Font Texture
			//Render text surface
			#pragma region Scenarios
			int scenario = 0;
			if (mFontOutline != 0 && mOutlineColor != Color(TRANSPARENT))
			{
				if (mFillColor != Color(TRANSPARENT))
					scenario = 0;
				else
					scenario = 1;
			}
			else
			{
				if (mFontOutline != 0)
					scenario = 2;
				else
					scenario = 3;
			}
			#pragma endregion
			TTF_Font *tempFont = NULL;
			if (scenario < 3)
			{
				tempFont = TTF_OpenFont(mPath.c_str(), mFontSize);
				TTF_SetFontStyle(tempFont, mFontStyle);
			}
			SDL_Surface *finalSurface = Create32BitRGBSurface(maxWidth, mHeight);
			Rect finalRect;
			//Iterate through characters, create surface for each character then blit it into final surface
			for (int i = 1; i < NUM_OF_PRINTABLE_CHARACTERS; i++) //space character has no width so skip it
			{
				std::string sym(1, PRINTABLE_CHARACTERS[i]);
				const char *character = sym.c_str();
				SDL_Surface *textSurface = NULL, *baseSurface = NULL, *dropShadowTextSurface = NULL, *dropShadowBaseSurface = NULL;
				if (scenario < 2) //outline
				{
					textSurface = TTF_RenderText_Blended(font, character, mOutlineColor);
					if (mShadowColor != NULL)
						dropShadowTextSurface = TTF_RenderText_Blended(font, character, *mShadowColor);
				}
				switch (scenario)
				{
					case 0:
						#pragma region Filled Outlined Font
						baseSurface = TTF_RenderText_Blended(tempFont, character, mFillColor);
						if (textSurface != NULL && baseSurface != NULL)
						{
							//Combine outline and fill
							Rect dstRect = { Round((baseSurface->w - textSurface->w) / 2.0f), Round((baseSurface->h - textSurface->h) / 2.0f), textSurface->w, textSurface->h };
							if (SDL_BlitSurface(textSurface, NULL, baseSurface, &dstRect) != 0)
								Error("Could not blit text surface to base surface!");
							Rect dstRect2 = { Round((textSurface->w - baseSurface->w) / 2.0f), Round((textSurface->h - baseSurface->h) / 2.0f), baseSurface->w, baseSurface->h };
							if (SDL_BlitSurface(baseSurface, NULL, textSurface, &dstRect2) != 0)
								Error("Could not blit base surface to text surface!");
						}
						else
							Error("Unable to render surface! SDL_ttf Error: " + std::string(TTF_GetError()));
						if (mShadowColor == NULL)
							break;
						dropShadowBaseSurface = TTF_RenderText_Blended(tempFont, character, *mShadowColor);
						if (dropShadowTextSurface != NULL && dropShadowBaseSurface != NULL)
						{
							//Combine outline and fill
							Rect dstRect = { Round((dropShadowBaseSurface->w - dropShadowTextSurface->w) / 2.0f), Round((dropShadowBaseSurface->h - dropShadowTextSurface->h) / 2.0f), dropShadowTextSurface->w, dropShadowTextSurface->h };
							if (SDL_BlitSurface(dropShadowTextSurface, NULL, dropShadowBaseSurface, &dstRect) != 0)
								Error("Could not blit text surface to base surface!");
							Rect dstRect2 = { Round((dropShadowTextSurface->w - dropShadowBaseSurface->w) / 2.0f), Round((dropShadowTextSurface->h - dropShadowBaseSurface->h) / 2.0f), dropShadowBaseSurface->w, dropShadowBaseSurface->h };
							if (SDL_BlitSurface(dropShadowBaseSurface, NULL, dropShadowTextSurface, &dstRect2) != 0)
								Error("Could not blit base surface to text surface!");
						}
						else
							Error("Unable to render surface! SDL_ttf Error: " + std::string(TTF_GetError()));
						break;
						#pragma endregion
					case 1:
						#pragma region Outlined Font
						break;
						#pragma endregion
					case 2:
						#pragma region Outlined Font But Transparent Outline Color
						//font just becomes fill font
						textSurface = TTF_RenderText_Blended(tempFont, character, mFillColor);
						if (mShadowColor != NULL)
							dropShadowTextSurface = TTF_RenderText_Blended(tempFont, character, *mShadowColor);
						break;
						#pragma endregion
					case 3:
						#pragma region Fill Font
						textSurface = TTF_RenderText_Blended(font, character, mFillColor);
						if (mShadowColor != NULL)
							dropShadowTextSurface = TTF_RenderText_Blended(font, character, *mShadowColor);
						break;
						#pragma endregion
				}
				if (textSurface != NULL)
				{
					Rect srcRect = AlphaTrimmedWidth(textSurface, i);
					if (mShadowColor != NULL)
					{
						SDL_SetSurfaceBlendMode(dropShadowTextSurface, SDL_BLENDMODE_NONE);
						SDL_SetSurfaceAlphaMod(dropShadowTextSurface, mShadowColor->a);
						finalRect = mCharRect[i];
						finalRect.x += Round((mCharRect[i].w - srcRect.w + mOffsetX) / 2.0f);
						finalRect.y += Round((mCharRect[i].h - textSurface->h + mOffsetY) / 2.0f);
						if (SDL_BlitSurface(dropShadowTextSurface, &srcRect, finalSurface, &finalRect) != 0)
							Error("Could not blit text surface to final surface!");
					}
					//Add character to final surface
					finalRect = mCharRect[i];
					finalRect.x += Round((mCharRect[i].w - (srcRect.w + mOffsetX)) / 2.0f);
					finalRect.y += Round((mCharRect[i].h - (textSurface->h + mOffsetY)) / 2.0f);
					if (SDL_BlitSurface(textSurface, &srcRect, finalSurface, &finalRect) != 0)
						Error("Could not blit text surface to final surface!");
				}
				else
					Error("Unable to render surface! SDL_ttf Error: " + std::string(TTF_GetError()));
				#pragma region Free Surfaces
				if (textSurface != NULL)
					SDL_FreeSurface(textSurface);
				if (baseSurface != NULL)
					SDL_FreeSurface(baseSurface);
				#pragma endregion
			}

			/*
			const char *fileName = (mPath + ToString(mFontSize) + ".bmp").c_str();
			printf("Saving Font: %s ====== %s\n", fileName, (SDL_SaveBMP(finalSurface, fileName) == 0) ? "Success!" : "Fail!"); //TEMP */

			if (finalSurface != NULL)
				mFont.SetTexture(RetrieveUniqueName(), finalSurface);
			else
				Error("Unable to render final surface! SDL_ttf Error: " + std::string(TTF_GetError()));
			if (scenario < 3)
				TTF_CloseFont(tempFont);
			TTF_CloseFont(font);
			#pragma endregion
		}
	}
	else
	{
		Error("FONT NOT LOADED!! " + mPath);
		//TODO: NOT YET IMPLEMENTED
	}
	mTexture = mFont.GetTexture();
}

void SpriteFont::DrawString(const std::string &text, const Point &dst, const Camera &camera)
{
	int length = text.size();
	Rect destination = { dst.x, dst.y, 0, 0 };
	Point cen = { 0, 0 };
	destination.x -= static_cast<int>(camera.position.x);
	destination.y -= static_cast<int>(camera.position.y);
	int firstX = destination.x;
	for (int i = 0, index = 0; i < length; i++)
	{
		index = text[i] - STARTING_CHARACTER_ASCII_CODE;
		if (index < 0 || index >= NUM_OF_PRINTABLE_CHARACTERS)
		{
			switch (text[i])
			{
				case '\n':
					destination.x = firstX;
					destination.y += mLineSkip;
					break;
				case '\t':
					destination.x += mTab;
					break;
			}
			continue;
		}
		destination.w = mCharRect[index].w;
		destination.h = mCharRect[index].h;
		SDL_RenderCopyEx(gRenderer, mTexture, &mCharRect[index], &destination, 0, &cen, SDL_FLIP_NONE);
		destination.x += mAdvance[index];
	}
}
void SpriteFont::DrawString(const std::string &text, const Point &dst, const Color &color, const Camera &camera)
{
	R_SetTextureColorMod(mTexture, color);
	int length = text.size();
	Rect destination = { dst.x, dst.y, 0, 0 };
	Point cen = { 0, 0 };
	destination.x -= static_cast<int>(camera.position.x);
	destination.y -= static_cast<int>(camera.position.y);
	int firstX = destination.x;
	for (int i = 0, index = 0; i < length; i++)
	{
		index = text[i] - STARTING_CHARACTER_ASCII_CODE;
		if (index < 0 || index >= NUM_OF_PRINTABLE_CHARACTERS)
		{
			switch (text[i])
			{
				case '\n':
					destination.x = firstX;
					destination.y += mLineSkip;
					break;
				case '\t':
					destination.x += mTab;
					break;
			}
			continue;
		}
		destination.w = mCharRect[index].w;
		destination.h = mCharRect[index].h;
		SDL_RenderCopyEx(gRenderer, mTexture, &mCharRect[index], &destination, 0, &cen, SDL_FLIP_NONE);
		destination.x += mAdvance[index];
	}
}
void SpriteFont::DrawString(const std::string &text, const Point &dst, const Point &center, const Color &color, const Camera &camera)
{
	R_SetTextureColorMod(mTexture, color);
	int length = text.size();
	Rect destination = { dst.x - center.x, dst.y - center.y, 0, 0 };
	Point cen = { 0, 0 };
	destination.x -= static_cast<int>(camera.position.x);
	destination.y -= static_cast<int>(camera.position.y);
	int firstX = destination.x;
	for (int i = 0, index = 0; i < length; i++)
	{
		index = text[i] - STARTING_CHARACTER_ASCII_CODE;
		if (index < 0 || index >= NUM_OF_PRINTABLE_CHARACTERS)
		{
			switch (text[i])
			{
				case '\n':
					destination.x = firstX;
					destination.y += mLineSkip;
					break;
				case '\t':
					destination.x += mTab;
					break;
			}
			continue;
		}
		destination.w = mCharRect[index].w;
		destination.h = mCharRect[index].h;
		SDL_RenderCopyEx(gRenderer, mTexture, &mCharRect[index], &destination, 0, &cen, SDL_FLIP_NONE);
		destination.x += mAdvance[index];
	}
}
void SpriteFont::DrawString(const std::string &text, const Point &dst, const Point &center, const Color &color, float scale, double angle, const Camera &camera)
{
	R_SetTextureColorMod(mTexture, color);
	int length = text.size(), pivotX = dst.x, pivotY = dst.y;
	Rect destination = { dst.x - Round(center.x * scale), dst.y - Round(center.y * scale), 0, 0 };
	Point cen = { 0, 0 };
	int camX = static_cast<int>(camera.position.x), camY = static_cast<int>(camera.position.y);
	destination.x -= camX;
	destination.y -= camY;
	pivotX -= camX, pivotY -= camY;
	int firstX = destination.x;
	for (int i = 0, index = 0; i < length; i++)
	{
		index = text[i] - STARTING_CHARACTER_ASCII_CODE;
		if (index < 0 || index >= NUM_OF_PRINTABLE_CHARACTERS)
		{
			switch (text[i])
			{
				case '\n':
					destination.x = firstX;
					if (scale == 1.0f)
						destination.y += mLineSkip;
					else
						destination.y += Round(mLineSkip * scale);
					break;
				case '\t':
					if (scale == 1.0f)
						destination.x += mTab;
					else
						destination.x += Round(mTab * scale);
					break;
			}
			continue;
		}
		if (scale == 1.0f)
		{
			destination.w = mCharRect[index].w;
			destination.h = mCharRect[index].h;
		}
		else
		{
			destination.w = Round(mCharRect[index].w * scale);
			destination.h = Round(mCharRect[index].h * scale);
		}
		cen.x = pivotX - destination.x;
		cen.y = pivotY - destination.y;
		SDL_RenderCopyEx(gRenderer, mTexture, &mCharRect[index], &destination, angle, &cen, SDL_FLIP_NONE);
		if (scale == 1.0f)
			destination.x += mAdvance[index];
		else
			destination.x += Round(mAdvance[index] * scale);
	}
}

const Point &SpriteFont::MeasureString(const std::string &text, bool saveLength)
{
	if (saveLength)
	{
		try
		{
			Point &measurement = mMeasuredStrings.at(text);
			return measurement;
		}
		catch (const std::exception &e)
		{
			e.what();
		}
	}
	else
	{
		if (mPrevMeasureString == text)
			return mMeasureStringHolder;
		mPrevMeasureString = text;
	}
	int width = 0, heightCount = 1, currentLength = 0, length = text.size();
	for (int i = 0, index = 0; i < length; i++)
	{
		index = text[i] - STARTING_CHARACTER_ASCII_CODE;
		if (index < 0 || index >= NUM_OF_PRINTABLE_CHARACTERS)
		{
			switch (text[i])
			{
				case '\n':
					heightCount++;
					width = std::max(width, currentLength);
					currentLength = 0;
					break;
				case '\t':
					currentLength += mTab;
					break;
			}
			continue;
		}
		if (length - 1 == 0) //a single character
			currentLength = mCharRect[index].w;
		else if (i == length - 1 || text[i + 1] == '\n')
			currentLength += Round((mCharRect[index].w + mAdvance[index]) / 2.0f);
		else
			currentLength += mAdvance[index];
	}
	width = std::max(width, currentLength);
	mMeasureStringHolder.Set(width, mCharRect[0].h * heightCount);
	if (saveLength)
		mMeasuredStrings[text] = mMeasureStringHolder;
	return mMeasureStringHolder;
}

std::string SpriteFont::FitText(const std::string &text, int width)
{
	std::string temp = text, result = "";
	while (MeasureString(temp).x > width)
	{
		for (int i = temp.size() - 2; i >= 0; i--)
		{
			if (MeasureString(temp.substr(0, i + 1)).x <= width)
			{
				for (int j = temp.substr(0, i + 1).size() - 1; j >= 0; j--)
				{
					if (temp[j] == ' ' || temp[j + 1] == ' ')
					{
						if (temp[j + 1] == ' ')
						{
							for (int k = j + 2, length = temp.size(); k < length; k++)
							{
								if (temp[k] != ' ')
								{
									j = k - 1;
									break;
								}
							}
						}
						result += temp.substr(0, j + 1) + "\n";
						temp = temp.substr(j + 1);
						break;
					}
				}
				break;
			}
		}
	}
	result += temp;
	return result;
}
Texture SpriteFont::CreateTexture(const std::string &text, Alignment alignment)
{
	std::vector<int> paddingLeft;
	std::vector<std::string> lines;
	Tokenize(text, lines, "\n");
	if (alignment != LEFT)
	{
		//calculate left side padding for each line of text
		int fullWidth = MeasureString(text).x;
		for (int i = 0, size = lines.size(); i < size; i++)
		{
			int pad = fullWidth - MeasureString(lines[i]).x;
			if (alignment == CENTER)
				pad /= 2;
			paddingLeft.push_back(pad);
		}
	}
	std::vector<Texture*> textures;
	std::vector<Rect> srcs, dsts;
	Rect destination = { 0, 0, 0, 0 };
	for (int l = 0, size = lines.size(); l < size; l++)
	{
		std::string &ln = lines[l];
		destination.x = 0;
		if (alignment != LEFT)
			destination.x += paddingLeft[l];
		for (int i = 0, index = 0, length = ln.size(); i < length; i++)
		{
			index = ln[i] - STARTING_CHARACTER_ASCII_CODE;
			if (index < 0 || index >= NUM_OF_PRINTABLE_CHARACTERS)
			{
				switch (ln[i])
				{
					case '\t':
						destination.x += mTab;
						break;
				}
				continue;
			}
			destination.w = mCharRect[index].w;
			destination.h = mCharRect[index].h;
			textures.push_back(&mFont);
			srcs.push_back(mCharRect[index]);
			dsts.push_back(destination);
			destination.x += mAdvance[index];
		}
		destination.y += mLineSkip;
	}
	return Texture::Combine(textures, srcs, dsts, 0, RetrieveUniqueName() + "##ALIGN:" + ToString(alignment) + "##TEXT:" + text);
}

const Color &SpriteFont::GetFillColor() const { return mFillColor; }
const Color &SpriteFont::GetOutlineColor() const { return mOutlineColor; }
const Texture &SpriteFont::GetTexture() const { return mFont; }
int SpriteFont::GetLineSkip() const { return mLineSkip; }

//////////////////
/// TextObject ///
//////////////////

const int TextObject::CEN_RIGHT = INT_MAX, TextObject::CEN_BOT = INT_MAX, TextObject::CEN_CENTER = INT_MIN;

void TextObject::Analyze(bool textChange)
{
	if (textChange)
	{
		mLinesSize = 0;
		mLines.clear();
		if (mFont == NULL)
			return;
		Tokenize(mString, mLines, "\n");
		mLinesSize = mLines.size();
		Point oDim = mDim;
		mDim = mFont->MeasureString(mString);
		if (oDim.x > 0)
		{
			mCen.x = Round(mDim.x * static_cast<float>(mCen.x) / oDim.x);
		}
		if (oDim.y > 0)
		{
			mCen.y = Round(mDim.y * static_cast<float>(mCen.y) / oDim.y);
		}
	}
	CalculateOffsets();
	CalculateCenters();
}

void TextObject::CalculateOffsets()
{
	if (mFont == NULL)
		return;
	mOffsets.clear();
	int lineSkip = mFont->GetLineSkip();
	for (int i = 0; i < mLinesSize; i++)
	{
		int padding = mDim.x - mFont->MeasureString(mLines[i]).x;
		switch (mAlignment)
		{
			case IFont::LEFT:
				padding = 0;
				break;
			case IFont::CENTER:
				padding /= 2;
				break;
		}
		mOffsets.push_back(Point(padding, i * lineSkip));
	}
}

void TextObject::CalculateCenters()
{
	if (mCen.x == CEN_RIGHT)
		mCen.x = mDim.x;
	else if (mCen.x == CEN_CENTER)
		mCen.x = mDim.x / 2;
	if (mCen.y == CEN_BOT)
		mCen.y = mDim.y;
	else if (mCen.y == CEN_CENTER)
		mCen.y = mDim.y / 2;
	mCenters.clear();
	for (int i = 0; i < mLinesSize; i++)
	{
		mCenters.push_back(Point(mCen.x - mOffsets[i].x, mCen.y - mOffsets[i].y));
	}
}

void TextObject::Draw(const Camera &camera)
{
	if (mFont != NULL)
	{
		for (int i = 0; i < mLinesSize; i++)
		{
			mFont->DrawString(mLines[i], mPos, mCenters[i], mColor, mScale, mAngle, camera);
		}
	}
}
