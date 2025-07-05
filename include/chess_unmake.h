#pragma once

#include "chess_piece_data.h"
#include "chess_game.h"

// struct to manage undo info
typedef struct UnmakeInfo{
    Move prevMove;
    Piece prevPiece;
    Piece capturedPiece;
    GameState prevGame;
    bool Promoted;
    struct UnmakeInfo* next;
} UnmakeInfo;

UnmakeInfo* push(UnmakeInfo* nextunmake, UnmakeInfo* unmakeStack);
UnmakeInfo* pop(UnmakeInfo* unmakeStack);