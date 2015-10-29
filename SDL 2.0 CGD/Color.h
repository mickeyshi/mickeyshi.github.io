#pragma once
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include <string>
#include "Enum.h"

class Color : public SDL_Color
{
public:
	//Default: 0, 0, 0, 0 
	Color(); 
	Color(Uint8 ir, Uint8 ig, Uint8 ib, Uint8 ia = 255);
	//Pick a preset color constant (i.e. WHITE, BLACK, RED, etc.)
	Color(EColor eColor, Uint8 ia = 255);
	//Pass in an array of 4 integers and they'll be used as the r, g, b, a respectively
	Color(Uint8 iArray[]);
	Color(const Color& copy);
	~Color();
	//Change color to the chosen preset color
	void Set(EColor eColor);
	//Set r, g, b, a to nr, ng, nb, na
	void Set(Uint8 nr, Uint8 ng, Uint8 nb, Uint8 na);
	//Add dr, dg, db, da to r, g, b, a
	void Add(Uint8 dr, Uint8 dg, Uint8 db, Uint8 da);
	Color *operator&();
	Color &operator=(const Color &rhs);
	bool operator==(const Color &rhs) const;
	bool operator!=(const Color &rhs) const;
	std::string ToString() const;
	std::string ToCSV() const;
};
