#include "Color.h"
#include "Conversion.h"
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include "Enum.h"

Color::Color()
{
	Set(0, 0, 0, 0);
}

Color::Color(Uint8 ir, Uint8 ig, Uint8 ib, Uint8 ia)
{
	Set(ir, ig, ib, ia);
}

Color::Color(EColor eColor, Uint8 ia)
{
	Set(eColor);
	if (a != 0) //if not TRANSPARENT
		a = ia;
}

Color::Color(Uint8 iArray[])
{
	Set(iArray[0], iArray[1], iArray[2], iArray[3]);
}

Color::Color(const Color &copy)
{
	Set(copy.r, copy.g, copy.b, copy.a);
}

Color::~Color()
{
}

void Color::Set(EColor eColor)
{
	int colorData = static_cast<int>(eColor);
	if (colorData != 0) //Not Transparent
	{
		int nr, ng, nb;
		nr = (colorData / 1000000) - 1000;
		ng = (colorData / 1000) - (1000 + nr) * 1000;
		nb = colorData - ((1000 + nr) * 1000 + ng) * 1000;
		Set(static_cast<Uint8>(nr), static_cast<Uint8>(ng), static_cast<Uint8>(nb), 255);
	}
	else
		Set(0, 0, 0, 0);
}
void Color::Set(Uint8 nr, Uint8 ng, Uint8 nb, Uint8 na) { r = nr; g = ng; b = nb; a = na; }
void Color::Add(Uint8 dr, Uint8 dg, Uint8 db, Uint8 da) { Set(r + dr, g + dg, b + db, a + da); }
Color *Color::operator&() { return this; }
Color &Color::operator=(const Color &rhs) { if (this != &rhs) { Set(rhs.r, rhs.g, rhs.b, rhs.a); } return *this; }
bool Color::operator==(const Color& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
bool Color::operator!=(const Color& rhs) const { return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a; }
std::string Color::ToString() const { return "(" + ::ToString(r) + ", " + ::ToString(g) + ", " + ::ToString(b) + ", " + ::ToString(a) + ")"; }
std::string Color::ToCSV() const { return ::ToString(r) + "," + ::ToString(g) + "," + ::ToString(b) + "," + ::ToString(a); }
