#pragma once

//Define if there are textures made within the program (i.e. textures that weren't loaded from a file)
#define TEXTURES_MADE_IN_GAME 

//Define if SDL_net will be used
#define NETWORK_CONNECTION

//Define if you want to use values in Defaults.h that represent abstract values such as DEFAULT_GAMEOBJECT_SRC_RECT, DEFAULT_GAMEOBJECT_DST_RECT and DEFAULT_OBJECT_CENTER
//This will apply to CGD classes/functions that interpret these values
//The constants themselves will then be taken literally
#define ABSTRACT_DEFAULTS

#ifndef RELEASE_MODE
//Define for Windowed or Fullscreen option on console (FOR TESTING PURPOSES)
#define TEMPORARY 
#endif

//Define for various testing purposes
//#define TESTING

//Define for Fullscreen to Windowed mode (and vice versa) using Alt+Enter
#define FULLSCREEN_WINDOWED_TOGGLE

#if _MSC_VER >= 1800
//If the round function already exists, this definition will stop the declaration in pymath.h
#define HAVE_ROUND
#endif

//Name that is shown on the top of window
#define GAME_NAME "GAME NAME"

//file name where the main application data is saved such as vertical sync
#define	PROGRAM_DATA_FILE "program"
//Resoultion width used to make the game in; use this for game coordinates regardless of screen resolution
#define RENDER_WIDTH 1920
//Resoultion height used to make the game in; use this for game coordinates regardless of screen resolution
#define RENDER_HEIGHT 1080
//Framerate the game was written in and what all the frame independent numbers are based off of (FIA() function uses ORIGINAL_FPS to adjust values for frame independence)
#define ORIGINAL_FPS 60
