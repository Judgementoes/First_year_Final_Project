#include "chess_game.h"

// for debugging purposes
void printGameState(GameState* game){
    if(game->turn == White) printf("White's turn\n");
    else printf("Black's turn\n");
    if(game->W_k) printf("White can castle king side\n");
    if(game->W_q) printf("White can castle queen side\n");
    if(game->B_k) printf("Black can castle king side\n");
    if(game->B_q) printf("Black can castle queen side\n");
    if(game->EnPassant.x < 8 && game->EnPassant.y < 8) printf("En Passant target square: %c%d\n", (char)(game->EnPassant.x+'a'), game->EnPassant.y+1);
    else printf("No en passant possible\n");
    printf("Halfmove clock: %d\n", game->HalfmoveClock);
    printf("Fullmove number: %d\n", game->FullmoveNum);
}

void printCheckStatus(GameState* game){
    printf("W: "); if(game->WhiteinCheck) printf("Check!\n"); else printf("Safe\n");
    printf("B: "); if(game->BlackinCheck) printf("Check!\n"); else printf("Safe\n");
}