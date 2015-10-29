#pragma once
#include <unordered_map>
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include "InputHandler.h"

#define ControlsMap std::unordered_map<Controls::Key, Controls::KeyInfo, std::hash<int>>

struct Controls
{
	//TODO: Add keys
	//define key enums here
	//then add to DEFAULT_CONTROLS
	enum Key : int { /*UP_KEY,*/ KEY_COUNT };
	struct KeyInfo
	{
		std::string NAME;
		SDL_Scancode KEY_CODE;

		KeyInfo() : KEY_CODE(SDL_NUM_SCANCODES) {}
		KeyInfo(const std::string &name, SDL_Scancode key) : NAME(name), KEY_CODE(key) {}
	};
	const static ControlsMap DEFAULT_CONTROLS;

	inline static void Set(const ControlsMap &keyMap)
	{
		mKeyMap.insert(keyMap.begin(), keyMap.end());
		Reload();
	}
	inline static void Set(Key k, const KeyInfo &ki)
	{
		mKeyMap[k] = ki;
		Reload();
	}
	inline static const KeyInfo &GetKeyInfo(Key key) { return mKeyMap.at(key); }
	inline static const ControlsMap &GetKeyMap() { return mKeyMap; }
	inline static bool ReleasePress(Key ck)
	{
		return KeyReleasePress(mKeyMap[ck].KEY_CODE);
	}
	inline static bool PressRelease(Key ck)
	{
		return KeyPressRelease(mKeyMap[ck].KEY_CODE);
	}
	inline static bool Held(Key ck)
	{
		return KeyHeld(mKeyMap[ck].KEY_CODE);
	}
private:
	Controls() { }
	static void Reload();
	static ControlsMap mKeyMap;
};

void SaveControls();
void LoadControls();
void DefaultControls();
int GetControlsKeyCount();