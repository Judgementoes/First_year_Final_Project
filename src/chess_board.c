#include "chess_board.h"

coord translateCoords(coord a){
    coord b;
    b.x = a.x*4+2;
    b.y = 15 - a.y*2;
    return b;
}

// Update graphical board based on chess board
void updateBoard(char board[18][35], Piece chessboard[8][8], coord PieceLocations[32]){
    
    // Wipe graphical board;
    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            coord pos = translateCoords((coord){j, i});
            board[pos.y][pos.x] = ' ';
        }
    }

    // Update piece position
    for(int i = 0; i < 32; i++){
        if(PieceLocations[i].x < 8 && PieceLocations[i].y < 8){
            coord pos = PieceLocations[i];
            coord newpos = translateCoords(pos);
            if(chessboard[pos.y][pos.x].color == White){
                switch(chessboard[pos.y][pos.x].type){
                    case Pawn: board[newpos.y][newpos.x] = 'P'; break;
                    case King: board[newpos.y][newpos.x] = 'K'; break;
                    case Bishop: board[newpos.y][newpos.x] = 'B'; break;
                    case Knight: board[newpos.y][newpos.x] = 'N'; break;
                    case Rook: board[newpos.y][newpos.x] = 'R'; break;
                    case Queen: board[newpos.y][newpos.x] = 'Q'; break;
                }
            } else{
                switch(chessboard[pos.y][pos.x].type){
                    case Pawn: board[newpos.y][newpos.x] = 'p'; break;
                    case King: board[newpos.y][newpos.x] = 'k'; break;
                    case Bishop: board[newpos.y][newpos.x] = 'b'; break;
                    case Knight: board[newpos.y][newpos.x] = 'n'; break;
                    case Rook: board[newpos.y][newpos.x] = 'r'; break;
                    case Queen: board[newpos.y][newpos.x] = 'q'; break;
                }
            }
        }
    }
}

// Display graphical board
void printBoard(char board[18][35], enum PieceColor turn){
    if(turn == White){        
        for(int i = 0; i<BV; i++){
            for(int j = 0; j<BH; j++){
                if(i == 17 || j == 34) printf("%c", board[i][j]);
                else{
                    if(board[i][j] == '-' || board[i][j] == '+' || board[i][j] == '|') printf(tMAGENTA "%c" tRESET, board[i][j]);
                    else if(board[i][j] >= 'A' && board[i][j] <= 'Z') printf(tCYAN "%c" tRESET, board[i][j]);
                    else printf(tRED "%c" tRESET, board[i][j]);
                }
            }
            printf("\n");
        }
    } else{
        for(int i = BV-1; i>=0; i--){
            for(int j = BH-1; j>=0; j--){ 
                if(i == 17 || j == 34) printf("%c", board[i][j]);
                else{
                    if(board[i][j] == '-' || board[i][j] == '+' || board[i][j] == '|') printf(tMAGENTA "%c" tRESET, board[i][j]);
                    else if(board[i][j] >= 'A' && board[i][j] <= 'Z') printf(tCYAN "%c" tRESET, board[i][j]);
                    else printf(tRED "%c" tRESET, board[i][j]);
                }
            }
            printf("\n");
        }
    }
}

// raylib helper functions
static inline bool Colorscmp(Color a, Color b){
    if(a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a) return true;
    else return false;
}

Vector2 TranslateCoord_V2(const coord a, const enum PieceColor turn){
    int rank = a.y;
    int file = a.x;
    if(turn == White){
        int x = 320 + file * 100 - 4;
        int y = 50 + (7-rank) * 100 - 6;
        return (Vector2){x, y};
    } else{
        int x = 320 + (7 - file) * 100 - 4;
        int y = 50 + rank * 100 - 6;
        return (Vector2){x, y};
    }
}

Vector2 TranslateCoord_V2_decorate(const coord a, const enum PieceColor turn){
    int rank = a.y;
    int file = a.x;
    if(turn == White){
        int x = 320 + file * 100;
        int y = 50 + (7-rank) * 100;
        return (Vector2){x, y};
    } else{
        int x = 320 + (7 - file) * 100;
        int y = 50 + rank * 100;
        return (Vector2){x, y};
    }
}

Vector2 TranslateCoord_V2_Float(float file, float rank, enum PieceColor turn){
    int squareSize = 100;
    int startposx = 320;
    int startposy = 50;
    float x, y;
    if(turn == White) {
        x = startposx + file * squareSize;
        y = startposy + (7 - rank) * squareSize;
    } else {
        x = startposx + (7 - file) * squareSize;
        y = startposy + rank * squareSize;
    }
    return (Vector2){x, y};
}

// raylib graphics render functions
void drawBoard(const enum PieceColor turn, Font font){
    int startposx = 320;
    int startposy = 50;
    const int fontSize = 24;
    if(turn == White){
        for(int rank = 7; rank >= 0; rank--){
            for(int file = 0; file < 8; file++){
                Color squareColor;
                // Draw the squares on the board
                if(rank%2 == 0){
                    squareColor = (file%2 != 0)? ChessWHITE:ChessBROWN;
                    DrawRectangle(startposx, startposy, 100, 100, squareColor);
                } else{
                    squareColor = (file%2 == 0)? ChessWHITE:ChessBROWN;
                    DrawRectangle(startposx, startposy, 100, 100, squareColor);
                }
                // Denote the files and ranks
                if(file == 0){
                    int rankposx = startposx+5;
                    int rankposy = startposy+5;
                    Color textColor;
                    if(Colorscmp(squareColor, ChessWHITE)) textColor = ChessBROWN;
                    else textColor = ChessWHITE;
                    char rankptr[2];
                    rankptr[0] = (char) rank + '1';
                    rankptr[1] = '\0';
                    DrawTextEx(font, rankptr, (Vector2){rankposx, rankposy}, fontSize, 0, textColor);
                }
                if(rank == 0){
                    int fileposx = startposx+82;
                    int fileposy = startposy+75;
                    Color textColor;
                    if(Colorscmp(squareColor, ChessWHITE)) textColor = ChessBROWN;
                    else textColor = ChessWHITE;
                    char fileptr[2];
                    fileptr[0] = (char) file + 'a';
                    fileptr[1] = '\0';
                    DrawTextEx(font, fileptr, (Vector2){fileposx, fileposy}, fontSize, 0, textColor);
                }
                startposx+=100;
            }
            startposy+=100;
            startposx = 320;
        }
    } else if(turn == Black){
        for(int rank = 0; rank < 8; rank++){
            for(int file = 7; file >=0; file--){
                Color squareColor;
                if(rank%2 == 0){
                    squareColor = (file%2 != 0)? ChessWHITE:ChessBROWN;
                    DrawRectangle(startposx, startposy, 100, 100, squareColor);
                } else{
                    squareColor = (file%2 == 0)? ChessWHITE:ChessBROWN;
                    DrawRectangle(startposx, startposy, 100, 100, squareColor);
                }
                if(file == 7){
                    int rankposx = startposx+5;
                    int rankposy = startposy+5;
                    Color textColor;
                    if(Colorscmp(squareColor, ChessWHITE)) textColor = ChessBROWN;
                    else textColor = ChessWHITE;
                    char rankptr[2];
                    rankptr[0] = (char) rank + '1';
                    rankptr[1] = '\0';
                    DrawTextEx(font, rankptr, (Vector2){rankposx, rankposy}, fontSize, 0, textColor);
                }
                if(rank == 7){
                    int fileposx = startposx+82;
                    int fileposy = startposy+75;
                    Color textColor;
                    if(Colorscmp(squareColor, ChessWHITE)) textColor = ChessBROWN;
                    else textColor = ChessWHITE;
                    char fileptr[2];
                    fileptr[0] = (char) file + 'a';
                    fileptr[1] = '\0';
                    DrawTextEx(font, fileptr, (Vector2){fileposx, fileposy}, fontSize, 0, textColor);
                }
                startposx+=100;
            }
            startposy+=100;
            startposx = 320;
        }
    }
}

void drawPieces(const Piece chessboard[8][8], const coord PieceLocations[32], const Texture2D pieceTextures[2][6], const enum PieceColor turn, const PieceAnimation animation, const bool dragging){
    const float scale = 0.23f;
    coord voidcoord = {8, 8};
    coord voidrook = {8, 8};
    if(animation.active && !animation.is_drag) voidcoord = animation.dst;
    if(dragging) voidcoord = animation.src;
    if(animation.castle) voidrook = animation.castler;
    for(int i = 0; i<32; i++){
        if(Coordscmp(PieceLocations[i], voidcoord) || Coordscmp(PieceLocations[i], voidrook)) continue;
        if(!Coordscmp(PieceLocations[i], (coord){8, 8})){
            Piece piece = chessboard[PieceLocations[i].y][PieceLocations[i].x];
            DrawTextureEx(pieceTextures[piece.color][piece.type], TranslateCoord_V2(PieceLocations[i], turn), 0.0f, scale, WHITE);
        }
    }
}

coord ScreentoBoard(const Vector2 mouse, const enum PieceColor turn){
    // References
    const int startposx = 320;
    const int startposy = 50;
    const int squareSize = 100;

    int relativex = mouse.x - 320;
    int relativey = mouse.y - 50;

    coord chesscoord = (coord){relativex/squareSize, relativey/squareSize};
    
    if(!checkBound(chesscoord)) return (coord){8, 8};

    if(turn == White){
        chesscoord.y = 7 - chesscoord.y;
    } else{
        chesscoord.x = 7 - chesscoord.x;
    }

    return chesscoord;
}

void drawHighlightedSquare(const Vector2 pos){
    DrawRectangle(pos.x, pos.y, 100, 100, Fade(ChessYELLOW, 0.45f));
}

void drawGrayBorder(const Vector2 pos){
    Vector2 innerpos = pos;
    for(int i = 0; i<20; i++){
        if(i == 0 || i == 19){
            for(int j = 0; j<20; j++) DrawRectangle(innerpos.x + 5*j, innerpos.y, 5, 5, ChessBorder);
        }
        else {
            DrawRectangle(innerpos.x, innerpos.y, 5, 5, ChessBorder);
            DrawRectangle(innerpos.x + 95, innerpos.y, 5, 5, ChessBorder);
        }
        innerpos.y += 5;
    }
}

void drawCaptureCircle(const Vector2 pos, const Color squareColor, const bool highlighted){
    DrawCircle(pos.x + 50, pos.y + 50, 50.0f, ChessGHOST);
    DrawCircle(pos.x + 50, pos.y + 50, 40.0f, squareColor);
    if(highlighted) DrawCircle(pos.x + 50, pos.y + 50, 40.0f, Fade(ChessYELLOW, 0.45f));
}

void drawSegregation(const Vector2 pos){
    Vector2 curpos = pos;
    for(int i = 0; i<3; i++){
        for(int j = 0; j<5; j++){
            DrawRectangle(curpos.x, curpos.y, 12, 2, GRAY);
            curpos.x += 22;
        }
        curpos.y += 100;
        curpos.x = pos.x;
    }
}

float lerp(int x, int y, float t){
    return x + (y - x) * t;
}

bool insufficientmaterialcheck(const Piece chessboard[8][8], const coord PieceLocations[32], const enum PieceColor turn){
    int bishopcount = 0;
    int knightcount = 0;
    int enemybishop = 0;
    int enemyknight = 0;
    int lightsquarebishops = 0;
    int darksquarebishops = 0;
    for(int i = 0; i<32; i++){
        if(!Coordscmp(PieceLocations[i], (coord){8, 8})){
            Piece piece = chessboard[PieceLocations[i].y][PieceLocations[i].x];
            if(piece.type == Rook || piece.type == Pawn || piece.type == Queen) return false; // any of these pieces are considered sufficient material
            if(piece.type == Bishop){
                if(piece.color == turn) bishopcount++;
                else enemybishop++;
                if(PieceLocations[i].y%2 == 0){
                    if(PieceLocations[i].x%2 == 0) darksquarebishops++;
                    else lightsquarebishops++;
                } else{
                    if(PieceLocations[i].x%2 == 0) lightsquarebishops++;
                    else darksquarebishops++;
                }
            }
            if(lightsquarebishops != 0 && darksquarebishops != 0) return false; // two or more bishops on different colored squares
            if(piece.type == Knight && piece.color == turn) knightcount++;
            if(piece.type == Knight && piece.color != turn) enemyknight++;
            // Bishops and/againts knights or 2 or more knights are sufficient
            if(knightcount + enemyknight + bishopcount > 1 || knightcount + enemyknight + enemybishop > 1) return false; 
        }
    }
    return true;
}

void drawCapturedPieces(const Piece chessboard[8][8], const coord PieceLocations[32], const Texture2D pieceTextures[2][6], const enum PieceColor turn){
    const float miniscale = 0.075f;
    // Captured black pieces
    int p = 0, b = 0, n = 0, r = 0, q = 0;
    // Captured white pieces
    int P = 0, B = 0, N = 0, R = 0, Q = 0;

    // Count all the pieces left
    for(int i = 0; i<32; i++){
        if(!Coordscmp(PieceLocations[i], (coord){8, 8})){
            Piece piece = chessboard[PieceLocations[i].y][PieceLocations[i].x];
            switch(piece.type){
                case Pawn: (piece.color == White)? P++ : p++; break;
                case Bishop: 
                    if(piece.color == White){
                        if(B < 2) B++;
                    } else{
                        if(b < 2) b++;
                    }
                    break;
                case Knight: 
                    if(piece.color == White){
                        if(N < 2) N++;
                    } else{
                        if(n < 2) n++;
                    }
                    break;
                case Rook: 
                    if(piece.color == White){
                        if(R < 2) R++;
                    } else{
                        if(r < 2) r++;
                    }
                    break;
                case Queen: 
                    if(piece.color == White){
                        if(Q < 1) Q++;
                    } else{
                        if(q < 1) q++;
                    }
                    break;
            }
        }
    }

    // Find the pieces captured by subtracting from the expected number
    Q = 1 - Q; q = 1 - q; R = 2 - R; r = 2 - r; N = 2 - N; n = 2 - n; B = 2 - B; b = 2 - b; P = 8 - P; p = 8 - p;

    int W_y = (turn == Black)? 853 : 13;
    int B_y = (turn == Black)? 13 : 853;
    int X = 320;

    // draw the black pawns
    for(int i = 0; i<p; i++){
        DrawTextureEx(pieceTextures[Black][Pawn], (Vector2){X+i*12, B_y}, 0.0f, miniscale, WHITE);  
    }
    
    // draw the white pawns
    for(int i = 0; i<P; i++){
        DrawTextureEx(pieceTextures[White][Pawn], (Vector2){X+i*12, W_y}, 0.0f, miniscale, WHITE);  
    }

    X = 430;
    // draw the black bishops
    for(int i = 0; i<b; i++){
        DrawTextureEx(pieceTextures[Black][Bishop], (Vector2){X+i*12, B_y}, 0.0f, miniscale, WHITE);
    }
    
    // draw the white bishops
    for(int i = 0; i<B; i++){
        DrawTextureEx(pieceTextures[White][Bishop], (Vector2){X+i*12, W_y}, 0.0f, miniscale, WHITE);  
    }

    X = 470;
    // draw the black knights
    for(int i = 0; i<n; i++){
        DrawTextureEx(pieceTextures[Black][Knight], (Vector2){X+i*12, B_y}, 0.0f, miniscale, WHITE);
    }
    
    // draw the white knights
    for(int i = 0; i<N; i++){
        DrawTextureEx(pieceTextures[White][Knight], (Vector2){X+i*12, W_y}, 0.0f, miniscale, WHITE);  
    }

    X = 510;
    // draw the black rooks
    for(int i = 0; i<r; i++){
        DrawTextureEx(pieceTextures[Black][Rook], (Vector2){X+i*23, B_y}, 0.0f, miniscale, WHITE);
    }
    
    // draw the white rooks
    for(int i = 0; i<R; i++){
        DrawTextureEx(pieceTextures[White][Rook], (Vector2){X+i*23, W_y}, 0.0f, miniscale, WHITE);  
    }

    X = 562;
    // draw the black queen
    for(int i = 0; i<q; i++){
        DrawTextureEx(pieceTextures[Black][Queen], (Vector2){X+i*12, B_y}, 0.0f, miniscale, WHITE);
    }
    
    // draw the white queen
    for(int i = 0; i<Q; i++){
        DrawTextureEx(pieceTextures[White][Queen], (Vector2){X+i*12, W_y}, 0.0f, miniscale, WHITE);  
    }

    // Calculate and print the material advantage
    int advantage = 9*(Q - q) + 5*(R - r) + 3*(N - n) + 3*(B - b) + (P - p);
    if(advantage != 0) DrawText(TextFormat("+%d", abs(advantage)), X + 40, (advantage > 0)? W_y + 11 : B_y + 11, 20, WHITE);
}