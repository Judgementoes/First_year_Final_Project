#pragma once
#include "chess_piece_data.h"
#include "chess_game.h"
#include <string.h>

// default board: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
static char* defaultPos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void readFEN(const char* FEN, Piece chessboard[8][8], coord PieceLocations[32], GameState* game);
void exportFEN(const Piece chessboard[8][8], const GameState* game, char* FEN);