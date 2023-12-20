#pragma once

#ifdef TICTACTOE_EXPORTS
#define TICTACTOE_API __declspec(dllexport)
#else
#define TICTACTOE_API __declspec(dllimport)
#endif

extern "C" TICTACTOE_API void print_empty();