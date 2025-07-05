#pragma once

#include "chess_piece_data.h"
#include "chess_game.h"
#include "chess_checksandpins.h"
#include "chess_unmake.h"

Move NotationtoMove(const char* N, char* PawnPromote);
static inline void expandMovelist(Movelist *moves);
static inline void GenerateSlidingMoves(Movelist *moves, coord src, Piece piece, Piece chessboard[8][8]);
static inline void GenerateKingMoves(Movelist *moves, coord src, Piece piece, Piece chessboard[8][8], GameState *game);
static inline void GenerateKnightMoves(Movelist *moves, coord src, Piece piece, Piece chessboard[8][8]);
static inline void GeneratePawnMoves(Movelist *moves, coord src, Piece piece, Piece chessboard[8][8], GameState *game);
static inline void FilterLegalMoves(Movelist* pseudoMoves, GameState * game, const coord MrPresidentSquares[16], const int MrPresidentcount);
void GenerateMoveList(Movelist *moves, Piece chessboard[8][8], coord PieceLocations[32], GameState *game);
void recordMove(UnmakeInfo** unmakeStack, const Move move, const Piece piece, const Piece Targetpiece, GameState* game, const char PawnPromote);
void makeMove(Piece chessboard[8][8], coord PieceLocations[32], Move move, GameState *game, const char PawnPromote, UnmakeInfo** unmakeStack);
void unmakeMove(UnmakeInfo** unmakeStack, GameState *game, Piece chessboard[8][8], coord PieceLocations[32]);
long long perftTest(Piece chessboard[8][8], coord PieceLocations[32], GameState *game, UnmakeInfo** unmakeStack, const int depth);