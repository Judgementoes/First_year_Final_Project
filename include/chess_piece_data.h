#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// Piece types
enum PieceType {
    None = 6,
    King = 0,
    Pawn = 1,
    Bishop = 2,
    Knight = 3,
    Rook = 4,
    Queen = 5
};

// Colors
enum PieceColor {
    White = 0, 
    Black = 1
};

// Struct to store types and colors of pieces
typedef struct Piece{
    enum PieceType type;
    enum PieceColor color;
} Piece;

// Struct to hold coordinates
typedef struct coord{
    int x;
    int y;
} coord;

// struct to denote a move
typedef struct Move{
    coord src;
    coord dst;
} Move;

// struct to hold a list of moves
typedef struct{
    Move* move;
    int count;
    int capacity;
}Movelist;

// struct to hold infomation of pinned pieces
typedef struct{
    coord dst[7]; // A pinned piece have at least 0 and at most 7 possible moves
    int movecount;
    coord src;
} pinnedPieces;

// Reference for sliding pieces
extern int NumSquarestoEdge[64][8];

// Reference for squares under attacked
extern bool BlackAttacking[64];
extern bool WhiteAttacking[64];

void stopwatch(time_t* start);
int lesser(int a, int b);
void Pre_generateMoveData(int NumSquarestoEdge[64][8]);
bool checkBound(coord c);
bool Coordscmp(coord a, coord b);
bool Movescmp(Move a, Move b);