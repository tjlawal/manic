#ifndef BASE_PROFILE_H
#define BASE_PROFILE_H

// Colour stuff, mostly for SuperLuminal.
#define _R(rgba) 	(((rgba) >> 16) & 0xFF)
#define _G(rgba)	(((rgba) >> 8) & 0xFF)
#define _B(rgba)	((rgba) & 0xFF)		

#define MakeColourFromRGB(R, G, B)	((((u32)(R)) << 24) | (((u32)(G)) << 16) | (((u32)(B)) << 8) | (u32)0xFF)
#define MakeRGBFromHex(hex)					MakeColourFromRGB(_R(hex), _G(hex), _B(hex))

#if BUILD_PROFILE
	#if PROFILER_SUPERLUMINAL
		#define ProfFunction()				PERFORMANCEAPI_INSTRUMENT_FUNCTION()
		#define ProfBlock(id, c) 			PERFORMANCEAPI_INSTRUMENT_COLOR(id, MakeRGBFromHex(c))
		#define ProfBlockData(id, d)	PERFORMANCEAPI_INSTRUMENT_DATA(id, d)
		#define ProfThreadName(name)	PerformanceAPI_SetCurrentThreadName(name)
	#elif PROFILER_TRACY
	#else
		#error "Profiler not recognized, are you using the correct profiler flag?"
	#endif
#else // Zero out defines
	#define ProfFunction()						(0)
	#define ProfBlock(id, c)					(0)
	#define ProfBlockData(id, d)			(0)
	#define ProfThreadName(name)			(0)
#endif

// Profiler Debug Colours
// Colour source: 
// https://upload.wikimedia.org/wikipedia/commons/2/2b/SVG_Recognized_color_keyword_names.svg
enum profDebugHexColours {
	profDebug_aliceblue 						=0xF0F8FF,
	profDebug_antiquewhite 					=0xFAEBD7,
	profDebug_aqua 									=0x00FFFF,
	profDebug_aquamarine 						=0x7FFFD4,
	profDebug_azure 								=0xF0FFFF,
	profDebug_beige 								=0xF5F5DC,
	profDebug_bisque 								=0xFFE4C4,
	profDebug_black 								=0x000000,
	profDebug_blanchedalmond 				=0xFFEBCD,
	profDebug_blue 									=0x0000FF,
	profDebug_blueviolet 						=0x8A2BE2,
	profDebug_brown 								=0xA52A2A,
	profDebug_burlywood 						=0xDEB887,
	profDebug_cadetblue 						=0x5F9EA0,
	profDebug_chartreuse 						=0x7FFF00,
	profDebug_chocolate 						=0xD2691E,
	profDebug_coral 								=0xFF7F50,
	profDebug_cornflowerblue 				=0x6495ED,
	profDebug_cornsilk 							=0xFFF8DC,
	profDebug_crimson 							=0xDC143C,
	profDebug_cyan 									=0x00FFFF,
	profDebug_darkblue 							=0x00008B,
	profDebug_darkcyan 							=0x008B8B,
	profDebug_darkgoldenrod 				=0xB8860B,
	profDebug_darkgray 							=0xA9A9A9,
	profDebug_darkgreen 						=0x006400,
	profDebug_darkgrey 							=0xA9A9A9,
	profDebug_darkkhaki 						=0xBDB76B,
	profDebug_darkmagenta 					=0x8B008B, // For all OS things
	profDebug_darkolivegreen 				=0x556B2F,
	profDebug_darkorange 						=0xFF8C00,
	profDebug_darkorchid 						=0x9932CC,
	profDebug_darkred 							=0x8B0000,
	profDebug_darksalmon 						=0xE9967A,
	profDebug_darkseagreen 					=0x8FBC8F,
	profDebug_darkslateblue 				=0x483D8B,
	profDebug_darkslategray 				=0x2F4F4F,
	profDebug_darkslategrey 				=0x2F4F4F,
	profDebug_darkturquoise 				=0x00CED1,
	profDebug_darkviolet 						=0x9400D3,
	profDebug_deeppink 							=0xFF1493,
	profDebug_deepskyblue 					=0x00BFFF,
	profDebug_dimgray 							=0x696969,
	profDebug_dimgrey 							=0x696969,
	profDebug_dodgerblue 						=0x1E90FF,
	profDebug_firebrick 						=0xB22222,
	profDebug_floralwhite 					=0xFFFAF0,
	profDebug_forestgreen 					=0x228B22,
	profDebug_fuchsia 							=0xFF00FF,
	profDebug_gainsboro 						=0xDCDCDC,
	profDebug_ghostwhite 						=0xF8F8FF,
	profDebug_gold 									=0xFFD700,
	profDebug_goldenrod 						=0xDAA520,
	profDebug_gray 									=0x808080,
	profDebug_grey 									=0x808080,
	profDebug_green 								=0x008000,
	profDebug_greenyellow 					=0xADFF2F,
	profDebug_honeydew 							=0xF0FFF0,
	profDebug_hotpink 							=0xFF69B4,
	profDebug_indianred 						=0xCD5C5C,
	profDebug_indigo 								=0x4B0082,
	profDebug_ivory 								=0xFFFFF0,
	profDebug_khaki 								=0xF0E68C,
	profDebug_lavender 							=0xE6E6FA,
	profDebug_lavenderblush 				=0xFFF0F5,
	profDebug_lawngreen 						=0x7CFC00,
	profDebug_lemonchiffon 					=0xFFFACD,
	profDebug_lightblue 						=0xADD8E6,
	profDebug_lightcoral 						=0xF08080,
	profDebug_lightcyan 						=0xE0FFFF,
	profDebug_lightgoldenrodyellow 	=0xFAFAD2,
	profDebug_lightgray 						=0xD3D3D3,
	profDebug_lightgreen 						=0x90EE90,
	profDebug_lightgrey 						=0xD3D3D3,
	profDebug_lightpink 						=0xFFB6C1,
	profDebug_lightsalmon 					=0xFFA07A,
	profDebug_lightseagreen 				=0x20B2AA,
	profDebug_lightskyblue 					=0x87CEFA,
	profDebug_lightslategray 				=0x778899,
	profDebug_lightslategrey 				=0x778899,
	profDebug_lightsteelblue 				=0xB0C4DE,
	profDebug_lightyellow 					=0xFFFFE0,
	profDebug_lime 									=0x00FF00,
	profDebug_limegreen 						=0x32CD32,
	profDebug_linen 								=0xFAF0E6,
	profDebug_magenta 							=0xFF00FF,
	profDebug_maroon 								=0x800000,
	profDebug_mediumaquamarine 			=0x66CDAA,
	profDebug_mediumblue 						=0x0000CD,
	profDebug_mediumorchid 					=0xBA55D3,
	profDebug_mediumpurple 					=0x9370DB,
	profDebug_mediumseagreen 				=0x3CB371,
	profDebug_mediumslateblue 			=0x7B68EE,
	profDebug_mediumspringgreen 		=0x00FA9A,
	profDebug_mediumturquoise 			=0x48D1CC,
	profDebug_mediumvioletred 			=0xC71585,
	profDebug_midnightblue 					=0x191970,
	profDebug_mintcream 						=0xF5FFFA,
	profDebug_mistyrose 						=0xFFE4E1,
	profDebug_moccasin 							=0xFFE4B5,
	profDebug_navajowhite 					=0xFFDEAD,
	profDebug_navy 									=0x000080,
	profDebug_oldlace 							=0xFDF5E6,
	profDebug_olive 								=0x808000,
	profDebug_olivedrab 						=0x6B8E23,
	profDebug_orange 								=0xFFA500,
	profDebug_orangered 						=0xFF4500,
	profDebug_orchid 								=0xDA70D6,
	profDebug_palegoldenrod 				=0xEEE8AA,
	profDebug_palegreen 						=0x98FB98,
	profDebug_paleturquoise 				=0xAFEEEE,
	profDebug_palevioletred 				=0xDB7093,
	profDebug_papayawhip 						=0xFFEFD5,
	profDebug_peachpuff 						=0xFFDAB9,
	profDebug_peru 									=0xCD853F,
	profDebug_pink 									=0xFFC0CB,
	profDebug_plum 									=0xDDA0DD,
	profDebug_powderblue 						=0xB0E0E6,
	profDebug_purple 								=0x800080,
	profDebug_red 									=0xFF0000,
	profDebug_rosybrown 						=0xBC8F8F,
	profDebug_royalblue 						=0x4169E1,
	profDebug_saddlebrown 					=0x8B4513,
	profDebug_salmon 								=0xFA8072,
	profDebug_sandybrown 						=0xF4A460,
	profDebug_seagreen 							=0x2E8B57,
	profDebug_seashell 							=0x2E8B57,
	profDebug_sienna 								=0xA0522D,
	profDebug_silver 								=0xC0C0C0,
	profDebug_skyblue 							=0x87CEEB,
	profDebug_slateblue 						=0x6A5ACD,
	profDebug_slategray 						=0x708090,
	profDebug_slategrey 						=0x708090,
	profDebug_snow 									=0xFFFAFA,
	profDebug_springgreen 					=0x00FF7F,
	profDebug_steelblue 						=0x4682B4,
	profDebug_tan 									=0xD2B48C,
	profDebug_teal 									=0x008080,
	profDebug_thistle 							=0xD8BFD8,
	profDebug_tomato 								=0xFF6347,
	profDebug_turquoise 						=0x40E0D0,
	profDebug_violet 								=0xEE82EE,
	profDebug_wheat 								=0xF5DEB3,
	profDebug_white 								=0xFFFFFF,
	profDebug_whitesmoke 						=0xF5F5F5,
	profDebug_yellow 								=0xFFFF00,
	profDebug_yellowgreen 					=0x9ACD32
};


#endif // BASE_PROFILE_H