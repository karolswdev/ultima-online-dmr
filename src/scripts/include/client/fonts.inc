//================================================================================
//--------------------------------------------------------------------------------
// Fonts
//--------------------------------------------------------------------------------
//================================================================================

const FONT_MAX                         := 0x09;   // Highest font.
const FONT_MIN                         := 0x00;   // Lowest font.

const FONT_BOLD                        := 0x00;   // Bold text.
const FONT_SHADOW                      := 0x01;   // Text with shadow.
const FONT_BOLD_SHAD                   := 0x02;   // Bold text with shadow.
const FONT_NORMAL                      := 0x03;   // Normal (default).
const FONT_GOTHIC                      := 0x04;   // Gothic.
const FONT_ITALIC                      := 0x05;   // Italic
const FONT_SMALL_DARK                  := 0x06;   // Small dark letters.
const FONT_COLOR                       := 0x07;   // Colorful font.
const FONT_RUNE                        := 0x08;   // Rune font (only capitals).
const FONT_SMALL_LIGHT                 := 0x09;   // Small light letters.

//================================================================================
//--------------------------------------------------------------------------------
// Colors
//--------------------------------------------------------------------------------
//================================================================================

const COLOR_DEFAULT                    := 0x0000;
const COLOR_BLACK                      := 0x0001;
const COLOR_INDIGO_DARK                := 0x0007;
const COLOR_INDIGO                     := 0x0008;
const COLOR_INDIGO_LIGHT               := 0x0009;

const COLOR_VIOLET_DARK                := 0x000c;
const COLOR_VIOLET                     := 0x000d;
const COLOR_VIOLET_LIGHT               := 0x000e;
const COLOR_MAGENTA_DARK               := 0x0011;
const COLOR_MAGENTA                    := 0x0012;
const COLOR_MAGENTA_LIGHT              := 0x0013;
const COLOR_RED_DARK                   := 0x0020;
const COLOR_RED_SPICED                 := 0x0021;
const COLOR_RED                        := 0x0028;
const COLOR_RED_LIGHT                  := 0x0029;
const COLOR_ORANGE_DARK                := 0x002a;
const COLOR_ORANGE                     := 0x002b;
const COLOR_ORANGE_LIGHT               := 0x002c;
const COLOR_YELLOW_DARK                := 0x0034;
const COLOR_YELLOW                     := 0x0035;
const COLOR_YELLOW_LIGHT               := 0x0036;
const COLOR_GREEN_DARK                 := 0x003e;
const COLOR_GREEN                      := 0x003f;
const COLOR_GREEN_LIGHT                := 0x0053;
const COLOR_CYAN_DARK                  := 0x0057;
const COLOR_CYAN                       := 0x0058;
const COLOR_CYAN_LIGHT                 := 0x0059;
const COLOR_BLUE_DARK                  := 0x0061;
const COLOR_BLUE                       := 0x005A;
const COLOR_BLUE_LIGHT                 := 0x0063;
const COLOR_GRAY_DARK                  := 0x0386;
const COLOR_GRAY                       := 0x0387;
const COLOR_GRAY_LIGHT                 := 0x0388;
const COLOR_BLACKMETAL                 := 0x044e;
const COLOR_GOLDMETAL                  := 0x046e;
const COLOR_BSHIMMER                   := 0x0480;  // T2A Only : Waterelemental color.
const COLOR_WHITE                      := 0x0481;  // T2A Only : True white.
const COLOR_STONE                      := 0x0482;  // T2A Only : Rock ( stoneharpy?)
const COLOR_SPECTRAL                   := 0x4631;  // Spectral
const COLOR_TRANSLUCENT                := 0x4001;  // Translucent

// VF backwards compatibility

const RED_COLOR 		       := COLOR_RED;
const GREEN_COLOR		       := COLOR_GREEN;

//================================================================================
//--------------------------------------------------------------------------------
// Font Colors
//--------------------------------------------------------------------------------
//================================================================================

const COLOR_BAD                        := COLOR_RED;
const COLOR_GOOD                       := COLOR_GREEN;

//--------------------------------------------------------------------------------
