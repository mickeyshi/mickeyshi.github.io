#include "Controls.h"
#include <string>
#include <map>
#include "Independent.h"

const std::string CONTROL_FILE = "Data/controls.dat";
ControlsMap Controls::mKeyMap;
const ControlsMap Controls::DEFAULT_CONTROLS = { /*{ UP_KEY, KeyInfo("UP", SDL_SCANCODE_UP) }*/ }; //TODO: Add default controls
const std::vector<SDL_Scancode> HARD_KEYS = { SDL_SCANCODE_ESCAPE }; //keys that don't get saved/loaded from controls.dat

void Controls::Reload()
{
	std::vector<SDL_Scancode> scancodes = HARD_KEYS;
	for (auto p : mKeyMap)
	{
		scancodes.push_back(p.second.KEY_CODE);
	}
	LoadSinglePressKeys(scancodes);
}

void SaveControls()
{
	std::map<std::string, std::string> controlData;
	const ControlsMap &KEY_MAP = Controls::GetKeyMap();
	for (auto p : KEY_MAP)
	{
		Controls::KeyInfo &ki = p.second;
		controlData[ki.NAME] = SDL_GetScancodeName(ki.KEY_CODE);
	}

	WriteFile(CONTROL_FILE, controlData, OVERWRITE_OR_NEW_FILE);
	LoadControls();
}

void LoadControls()
{
	//names from https://wiki.libsdl.org/SDL_Scancode
	std::map<std::string, std::string> controlData;
	ControlsMap keyMap;
	ColumnData(controlData, CONTROL_FILE, true);

	for (auto p : Controls::DEFAULT_CONTROLS)
	{
		Controls::KeyInfo &ki = p.second;
		Controls::Key key = p.first;
		try
		{
			std::string keyName = controlData.at(ki.NAME);
			keyMap[key] = Controls::KeyInfo(ki.NAME, SDL_GetScancodeFromName(keyName.c_str()));
		}
		catch (const std::exception &e)
		{
			Error("LoadControls: " + std::string(e.what()));
		}
	}

	Controls::Set(keyMap);
}

void DefaultControls()
{
	Controls::Set(Controls::DEFAULT_CONTROLS);
}

int GetControlsKeyCount() { return static_cast<int>(Controls::KEY_COUNT); }
