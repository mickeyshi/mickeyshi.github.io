#include "DataObject.h"
#include "Level.h"
#include "Parser.h"

DataObj::~DataObj() {}

/////////////////
/// SoundData ///
/////////////////

void SoundData::Load(Level* lvl, Parser::Arguments &args, const std::string& name)
{
	std::string path;
	args.GetArg(path, GetVarName(path), path);
	int channel = -1;
	args.GetArg(channel, GetVarName(channel), channel);
	lvl->AddDataObject(name, new SoundData(path, channel));
}

//////////////////////
/// SpriteFontData ///
//////////////////////

void SpriteFontData::Load(Level* lvl, Parser::Arguments &args, const std::string& name)
{
	std::string path;
	args.GetArg(path, GetVarName(path), path);
	int fontSize = 30, dropShadowOffsetX = 0, dropShadowOffsetY = 0, fontOutline = 0, fontStyle = 0;
	Color dropShadowColor, fillColor, outlineColor;

	args.GetArg(fontSize, GetVarName(fontSize));
	args.GetArg(dropShadowOffsetX, GetVarName(dropShadowOffsetX));
	args.GetArg(dropShadowOffsetY, GetVarName(dropShadowOffsetY));
	args.GetArg(fontOutline, GetVarName(fontOutline));
	args.GetArg(fontStyle, GetVarName(fontStyle));
	args.GetArg(dropShadowColor, GetVarName(dropShadowColor));
	args.GetArg(fillColor, GetVarName(fillColor));
	args.GetArg(outlineColor, GetVarName(outlineColor));

	if (dropShadowOffsetX == 0 && dropShadowOffsetY == 0)
		lvl->AddDataObject(name, new SpriteFontData(new SpriteFont(path, fontSize, fontOutline, fontStyle, fillColor, outlineColor)));
	else
		lvl->AddDataObject(name, new SpriteFontData(new SpriteFont(path, fontSize, dropShadowOffsetX, dropShadowOffsetY, fontOutline, fontStyle, dropShadowColor, fillColor, outlineColor)));
}

////////////////////
/// GraphicsData ///
////////////////////

GraphicsData::GraphicsData()
	: STATIC_SRC_RECT(DEFAULT_GAMEOBJECT_SRC_RECT), STATIC(true), FLIP(SDL_FLIP_NONE)
{
}

void GraphicsData::Load(Level* lvl, Parser::Arguments &args, const std::string& name)
{
	Texture texture;
	std::string txture;
	args.GetArg(txture, GetVarName(texture));
	if (txture.empty())
	{
		Error("ERROR: No texture provided for GraphicsData! name = " + name);
		return;
	}
	if (CharCount(txture, ':') > 0)
	{
		Color txtureColor;
		FromString(txtureColor, Replace(txture, ":", ","));
		texture.SetTexture(Rect(0, 0, 1, 1), txtureColor);
	}
	else
	{
		texture.SetTexture(txture);
	}

	Color color = args.GetArg<Color>(GetVarName(color), DEFAULT_COLOR);
	SDL_RendererFlip flip = args.GetArg<SDL_RendererFlip>(GetVarName(flip), SDL_FLIP_NONE);
	Rect staticSrcRect = args.GetArg<Rect>("srcRect", DEFAULT_GAMEOBJECT_SRC_RECT);
	Defaults_Evaluate(texture, staticSrcRect);
	std::vector<Animation> animations;
	Point frames(-1, -1);
	args.GetArg(frames, GetVarName(frames));
	if (frames.x >= 0)
	{
		int delay = args.GetArg(GetVarName(delay), 1);
		Point offsetInc = args.GetArg(GetVarName(offsetInc), Point(0, 0));
		int numFrames = args.GetArg(GetVarName(numFrames), frames.x * frames.y);
		animations.push_back(Animation(staticSrcRect, frames, numFrames, delay, offsetInc));
	}

	int i = 0;
	while (true)
	{
		std::string strI = ToString(i);
		frames.Set(-1, -1);
		args.GetArg(frames, GetVarName(frames) + strI);
		if (frames.x < 0)
			break;
		Rect srcRect = args.GetArg(GetVarName(srcRect) + strI, staticSrcRect);
		Defaults_Evaluate(texture, srcRect);
		int delay = args.GetArg(GetVarName(delay) + strI, 1);
		Point offsetInc = args.GetArg(GetVarName(offsetInc) + strI, Point(0, 0));
		int numFrames = args.GetArg(GetVarName(numFrames) + strI, frames.x * frames.y);
		i++;
		animations.push_back(Animation(srcRect, frames, numFrames, delay, offsetInc));
	}
	lvl->AddDataObject(name, new GraphicsData(texture, animations, staticSrcRect, color, flip, animations.empty()));
}
