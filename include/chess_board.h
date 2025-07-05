#pragma once

#include "chess_piece_data.h"
#include "raylib.h"

// ANSI color codes
#define tRED     "\033[1;31m"
#define tGREEN   "\033[1;32m"
#define tYELLOW  "\033[1;33m"
#define tBLUE    "\033[1;34m"
#define tMAGENTA "\033[1;35m"
#define tCYAN    "\033[1;36m"
#define tRESET   "\033[0m"

#define BH 35
#define BV 18

#define ChessBROWN CLITERAL(Color){185, 135, 99, 255}
#define ChessWHITE CLITERAL(Color){236, 214, 177, 255}
#define ChessYELLOW CLITERAL(Color){246, 234, 115, 255}
#define ChessBorder CLITERAL(Color){253, 249, 206, 255}
#define ChessGHOST CLITERAL(Color){77, 77, 77, 77}
#define ChessBackground CLITERAL(Color){63, 73, 89, 255}
#define ChessMenu CLITERAL(Color){15, 66, 107, 255}

// Struct to store relevant animation data
typedef struct{
    bool active;
    bool castle;
    bool uncastle;
    bool is_drag;
    coord src, dst;
    coord castler;
    float t;
    Piece piece;
    Piece Targetpiece;
} PieceAnimation;

// Struct to hold clock data
typedef struct{
    double time;
    double pause;
    bool ticktock; // No brainrot
} ChessClock;

float lerp(int x, int y, float t);
coord translateCoords(coord a);
Vector2 TranslateCoord_V2(const coord a, const enum PieceColor turn);
Vector2 TranslateCoord_V2_decorate(const coord a, const enum PieceColor turn);
Vector2 TranslateCoord_V2_Float(float file, float rank, enum PieceColor turn);
void updateBoard(char board[18][35], Piece chessboard[8][8], coord PieceLocations[32]);
void printBoard(char board[18][35], enum PieceColor turn);
static inline bool Colorscmp(Color a, Color b);
void drawBoard(enum PieceColor turn, Font font);
void drawPieces(const Piece chessboard[8][8], const coord PieceLocations[32], const Texture2D pieceTextures[2][6], const enum PieceColor turn, const PieceAnimation animation, const bool dragging);
coord ScreentoBoard(const Vector2 mouse, const enum PieceColor turn);
void drawHighlightedSquare(const Vector2 pos);
void drawGrayBorder(const Vector2 pos);
void drawCaptureCircle(const Vector2 pos, const Color squareColor, const bool highlighted);
void drawSegregation(const Vector2 pos);
bool insufficientmaterialcheck(const Piece chessboard[8][8], const coord PieceLocations[32], const enum PieceColor turn);
void drawCapturedPieces(const Piece chessboard[8][8], const coord PieceLocations[32], const Texture2D pieceTextures[2][6], const enum PieceColor turn);