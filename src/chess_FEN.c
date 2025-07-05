#include "chess_FEN.h"

// Translate FEN to chessboard
void readFEN(const char* FEN, Piece chessboard[8][8], coord PieceLocations[32], GameState* game){

    // 2 arrays to keep track of the positions of the pieces of each color
    for(int i = 0; i<32; i++){
        PieceLocations[i] = (coord){8, 8};
    }
    
    int n = strlen(FEN);
    int rank = 7, file = 0;
    int countPieces = 0, countSpace = 0;
    for(int i = 0; i<n; i++){
        if(FEN[i] == ' '){
            countSpace++;
            continue;
        }
        if(countSpace == 0){    
            if(FEN[i] >= 'A'){
                PieceLocations[countPieces] = (coord){file, rank};
                countPieces++;
                if(FEN[i] >= 'a'){
                    chessboard[rank][file].color = Black;
                    switch(FEN[i]){
                        case 'p': chessboard[rank][file].type = Pawn; break;
                        case 'k': chessboard[rank][file].type = King; game->BlackKingIdx = rank*8 + file; break;
                        case 'b': chessboard[rank][file].type = Bishop; break;
                        case 'n': chessboard[rank][file].type = Knight; break;
                        case 'r': chessboard[rank][file].type = Rook; break;
                        case 'q': chessboard[rank][file].type = Queen; break;
                    }
                } else {
                    chessboard[rank][file].color = White;
                    switch(FEN[i]){
                        case 'P': chessboard[rank][file].type = Pawn; break;
                        case 'K': chessboard[rank][file].type = King; game-> WhiteKingIdx = rank*8 + file; break;
                        case 'B': chessboard[rank][file].type = Bishop; break;
                        case 'N': chessboard[rank][file].type = Knight; break;
                        case 'R': chessboard[rank][file].type = Rook; break;
                        case 'Q': chessboard[rank][file].type = Queen; break;
                    }
                }
                file++;
            } else if(FEN[i] >= '1'){
                file+=(int)(FEN[i] - '0');
            } else{
                rank--;
                file=0;
            }
        }
        else if(countSpace == 1){
            if(FEN[i] == 'w') game->turn = White; else game->turn = Black;
        }
        else if(countSpace == 2){
            if(FEN[i] == '-'){
                game->B_k = false;
                game->B_q = false;
                game->W_k = false;
                game->W_q = false;
            } else{
                switch(FEN[i]){
                    case 'K': game->W_k = true; break;
                    case 'Q': game->W_q = true; break;
                    case 'k': game->B_k = true; break;
                    case 'q': game->B_q = true; break;
                }
            }
        } else if(countSpace == 3){
            if(FEN[i] == '-') game->EnPassant = (coord){8, 8};
            else{
                int x = (int)FEN[i] - 'a';
                int y = (int)FEN[i+1] - '1';
                game->EnPassant = (coord){x, y};
                i++;
            }
        } else if(countSpace == 4){
            // Parse halfmove clock
            int val = 0;
            while(i < n && FEN[i] >= '0' && FEN[i] <= '9') {
                val = val * 10 + (FEN[i] - '0');
                i++;
            }
            game->HalfmoveClock = val;
            i--;
        } else if(countSpace == 5){
            // Parse fullmove number
            int val = 0;
            while(i < n && FEN[i] >= '0' && FEN[i] <= '9') {
                val = val * 10 + (FEN[i] - '0');
                i++;
            }
            game->FullmoveNum = val;
            i--;
        }
    }
}

void exportFEN(const Piece chessboard[8][8], const GameState* game, char* FEN){
    int FENcount = 0;

    // Load the piece position
    for(int rank = 7; rank >= 0; rank--){
        int blankspace = 0;
        for(int file = 0; file < 8; file++){
            Piece piece = chessboard[rank][file];
            if(piece.type == None) blankspace++; // count the squares with no pieces
            else{
                if(blankspace > 0){
                    FEN[FENcount++] = (char)('0' + blankspace); // add in the number of blank spaces
                    blankspace = 0;
                }
                if(piece.color == White){
                    switch(piece.type){
                        case Pawn:   FEN[FENcount++] = 'P'; break;
                        case King:   FEN[FENcount++] = 'K'; break;
                        case Queen:  FEN[FENcount++] = 'Q'; break;
                        case Rook:   FEN[FENcount++] = 'R'; break;
                        case Bishop: FEN[FENcount++] = 'B'; break;
                        case Knight: FEN[FENcount++] = 'N'; break;
                        default: break;
                    }
                } else if(piece.color == Black){
                    switch(piece.type){
                        case Pawn:   FEN[FENcount++] = 'p'; break;
                        case King:   FEN[FENcount++] = 'k'; break;
                        case Queen:  FEN[FENcount++] = 'q'; break;
                        case Rook:   FEN[FENcount++] = 'r'; break;
                        case Bishop: FEN[FENcount++] = 'b'; break;
                        case Knight: FEN[FENcount++] = 'n'; break;
                        default: break;
                    }
                }
            }
        }
        if(blankspace > 0){
            FEN[FENcount++] = (char)('0' + blankspace); // add in the rest of the blank spaces (if any)
            blankspace = 0;
        }
        if(rank != 0) FEN[FENcount++] = '/';
    }

    FEN[FENcount++] = ' '; // add a space between each section

    // Denote who's turn it is
    FEN[FENcount++] = (game->turn == White)? 'w':'b';

    FEN[FENcount++] = ' ';

    // Which castling move is still valid
    if(game->W_k) FEN[FENcount++] = 'K';
    if(game->W_q) FEN[FENcount++] = 'Q';
    if(game->B_k) FEN[FENcount++] = 'k';
    if(game->B_q) FEN[FENcount++] = 'q';
    if(!game->W_k && !game->W_q && !game->B_k && !game->B_q) FEN[FENcount++] = '-';
    FEN[FENcount++] = ' ';

    // Add in En Passant target square
    if(!Coordscmp(game->EnPassant, (coord){8, 8})){
        FEN[FENcount++] = 'a' + game->EnPassant.x;
        FEN[FENcount++] = '1' + game->EnPassant.y;
    } else FEN[FENcount++] = '-';
    FEN[FENcount++] = ' ';
    
    // Halfmove clock
    FENcount += sprintf(FEN + FENcount, "%d ", game->HalfmoveClock);

    // Fullmove number (no trailing space)
    FENcount += sprintf(FEN + FENcount, "%d", game->FullmoveNum);

    // Null-terminate and return a copy
    FEN[FENcount] = '\0';
}