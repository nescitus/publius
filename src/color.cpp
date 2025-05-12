#include "types.h"

Color operator~(Color c) {
    return Color(c ^ Black);
}

Color operator++(Color& d) {
    return d = Color(int(d) + 1);
}