#include "chess_piece_data.h"

// Reference for sliding pieces
int NumSquarestoEdge[64][8];

// Reference for squares under attacked
bool BlackAttacking[64];
bool WhiteAttacking[64];

// performance testing function
void stopwatch(time_t* start){
    time_t end = clock();
    printf("%lf ms\n", (double)end - *start);
    *start = clock();
}

int lesser(int a, int b){
    if(a > b) return b;
    else return a;
}

void Pre_generateMoveData(int NumSquarestoEdge[64][8]){
    for(int rank = 0; rank<8; rank++){
        for(int file = 0; file<8; file++){
            int squareIdx = rank*8+file;
            int up = rank;
            int down = 7 - rank;
            int left = file;
            int right = 7 - file;
            NumSquarestoEdge[squareIdx][0] = up;
            NumSquarestoEdge[squareIdx][1] = down;
            NumSquarestoEdge[squareIdx][2] = left;
            NumSquarestoEdge[squareIdx][3] = right;
            NumSquarestoEdge[squareIdx][4] = lesser(up, right);
            NumSquarestoEdge[squareIdx][5] = lesser(up, left);
            NumSquarestoEdge[squareIdx][6] = lesser(down, left);
            NumSquarestoEdge[squareIdx][7] = lesser(down, right);
        }
    }
}

bool checkBound(coord c){
    if(c.x < 8 && c.x>=0 && c.y < 8 && c.y>=0) return true;
    return false;
}

bool Coordscmp(coord a, coord b){
    if(a.x == b.x && a.y == b.y) return true;
    return false;
}

bool Movescmp(Move a, Move b){
    if(Coordscmp(a.src, b.src) && Coordscmp(a.dst, b.dst)) return true;
    return false;
}