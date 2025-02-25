/*
Define Color enum and what can be done with it.

This follows the concept of *type safety*. The engine
would work just fine using int instead of Color, but
coding it with user-defined types is much safer. You
will get a compiler error when attempting to use
Color instead of Square or trying to add two Color
variables together. In fact, there are only two things
you can fo with Color: flip it so that White becomes
Black or another way round using ~operator or use 
++ operator to traverse the loop and do something
both with white and black pieces.

For those unfamiliar with the convention, operators
can be defined just like functions.

*/

#pragma once

enum Color { White, Black, colorNone };

Color operator~(Color c);   // switch color
Color operator++(Color& d); // increment color in a "for" loop