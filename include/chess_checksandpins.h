#pragma once

#include "chess_piece_data.h"
#include "chess_game.h"

static inline void MarkMrPresidentSquares(const enum PieceType type, coord MrPresidentSquares[16], int *MrPresidentcount, const int KingSquare, const int EnemySquare, const int dirIdx);
static inline void MarkSlidingMoves(const Piece piece, const coord src, const Piece chessboard[8][8]);
static inline void MarkBasicMoves(const Piece piece, const coord src, const Piece chessboard[8][8]);
static inline void updateAttackedSquares(Piece chessboard[8][8], const coord PieceLocations[32], const GameState *game);
void checkforChecks(Piece chessboard[8][8], coord PieceLocations[32], GameState *game);
void findAssassins(const Piece chessboard[8][8], const GameState *game, int *assassins, coord MrPresidentSquares[16], int *MrPresidentcount);
static inline void MarkPinnedMoves(GameState *game, const coord src, const int kingSquare, const int enemySquare, const int dirIdx);
void updatePinnedPieces(const Piece chessboard[8][8], GameState *game);