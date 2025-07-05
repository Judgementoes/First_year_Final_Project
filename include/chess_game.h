#pragma once

#include "chess_piece_data.h"

// struct to manage game state logic
typedef struct{
    enum PieceColor turn;
    enum PieceColor firstToMove;
    bool W_q, W_k;
    bool B_q, B_k;
    coord EnPassant;
    int HalfmoveClock;
    int FullmoveNum;
    int BlackKingIdx;
    int WhiteKingIdx;
    bool BlackinCheck;
    bool WhiteinCheck;
    // Kings can be pinned in maximum 8 directions (highly unlikely)
    pinnedPieces pWPieces[8];
    pinnedPieces pBPieces[8];
    int pinnedWcount;
    int pinnedBcount;
    // TBA
} GameState;

void printGameState(GameState* game);
void printCheckStatus(GameState* game);