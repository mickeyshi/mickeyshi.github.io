#pragma once
#include "Texture.h"
#include "Rect.h"
#include "Color.h"
#include "Point.h"
#include "Font.h"
#include "Parser.h"
#include "Level.h"
#include <vector>
#include <string>

class Level;

class DataObj {
public:
    virtual ~DataObj() = 0;
};

class SoundData : public DataObj
{
public:
	const std::string PATH;
	const int CHANNEL; //used by SoundEffect only, -1 by default

	SoundData(const std::string &pth, int chnl = -1) : PATH(pth), CHANNEL(chnl) {}
	static void Load(Level* lvl, Parser::Arguments &args, const std::string& name);
};

class SpriteFontData : public DataObj
{
public:
	SpriteFont *font;

	SpriteFontData(SpriteFont *f = NULL) : font(f) {}
	~SpriteFontData() { delete font; }
	static void Load(Level* lvl, Parser::Arguments &args, const std::string& name);
};

template <class T>
class VectorData : public DataObj
{
public:
	const std::vector<T> DATA;

	VectorData(const std::vector<T> &data) : DATA(data) {}
	inline static void Load(Level* lvl, Parser::Arguments &args, const std::string& name)
	{
		std::vector<T> data = args.GetVector<T>(GetVarName(data));
		lvl->AddDataObject(name, new VectorData<T>(data));
	}
};

class GraphicsData : public DataObj {
public:
	struct Animation {
		const Rect SRC;
		const Point FRAMES, OFFSET_INC;
		const int NUM_FRAMES, DELAY;
		Animation(const Rect &src, const Point &frames, int numFrames, int delay, const Point &offsetInc)
			: SRC(src), FRAMES(frames), OFFSET_INC(offsetInc), NUM_FRAMES(numFrames), DELAY(delay) {}
	};
	const Texture TEXTURE;
	const std::vector<Animation> ANIMATIONS;
	const Rect STATIC_SRC_RECT;
	const Color COLOR;
	const SDL_RendererFlip FLIP;
	const bool STATIC;

	GraphicsData();
	GraphicsData(const Texture &texture, const std::vector<Animation> &animations, const Rect &staticSrcRect, const Color &color, SDL_RendererFlip flip, bool staticSrc)
		: TEXTURE(texture), ANIMATIONS(animations), STATIC_SRC_RECT(staticSrcRect), COLOR(color), FLIP(flip), STATIC(staticSrc) {}
	static void Load(Level* lvl, Parser::Arguments &args, const std::string& name);
};

/*
 class NewDataObj : public DataObj
 {
 public:
 private:
 };
 */