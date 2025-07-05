#include "chess_moves.h"

// Turn notations into moves
Move NotationtoMove(const char* N, char* PawnPromote){
    Move move;
    *PawnPromote = '\0';
    move.src = (coord){(int)N[0] - 'a', (int)N[1] - '1'};
    move.dst = (coord){(int)N[2] - 'a', (int)N[3] - '1'};
    if(N[4] != '\0') {
        if(N[4] == 'b' || N[4] == 'n' || N[4] == 'r' || N[4] == 'q'){
            *PawnPromote = N[4];
        } else {
            *PawnPromote = '-';
        }
    }
    return move;
}

static inline void expandMovelist(Movelist *moves){
    moves->capacity*=2;
    Move *tmp = (Move* )realloc(moves->move, moves->capacity*sizeof(Move));
    if(tmp == NULL){
        fprintf(stderr, "Failed to expand movelist\n");
        exit(EXIT_FAILURE);
    }
    moves->move = tmp;
}

// Generate all moves for all sliding pieces
static inline void GenerateSlidingMoves(Movelist *moves, coord src, Piece piece, Piece chessboard[8][8]){
    // Direction offset for vertical, horizontal and diagonal directions
    int dx[8] = {0, 0, -1, 1, 1, -1, -1, 1};
    int dy[8] = {-1, 1, 0, 0, -1, -1, 1, 1};

    // Logic to filter bishops, rooks, and queens
    int Start = (piece.type == Bishop)? 4:0;
    int End = (piece.type == Rook)? 4:8;
    
    int squareIdx = src.y * 8 + src.x;
    for(int dirIdx = Start; dirIdx < End; dirIdx++){
        for(int traverseIdx = 0; traverseIdx < NumSquarestoEdge[squareIdx][dirIdx]; traverseIdx++){
            coord dst = (coord){src.x + dx[dirIdx] * (traverseIdx+1), src.y + dy[dirIdx] * (traverseIdx+1)};
            Piece Targetpiece = chessboard[dst.y][dst.x];

            // Blocked by a friendy piece -> can't continue
            if(Targetpiece.type != None && Targetpiece.color == piece.color) break;

            // Add move to movelist
            if(moves->count >= moves->capacity) expandMovelist(moves);
            moves->move[moves->count] = (Move){src, dst};
            moves->count++;

            // Captured an enemy piece -> add move to list but can't continue
            if(Targetpiece.type != None && Targetpiece.color != piece.color) break;
        }
    }
}

// Generate all moves for the King
static inline void GenerateKingMoves(Movelist *moves, coord src, Piece piece, Piece chessboard[8][8], GameState *game){
    // Direction offset for vertical, horizontal and diagonal directions
    int dx[8] = {0, 0, -1, 1, 1, -1, -1, 1};
    int dy[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
    
    // Standard moves
    for(int i = 0; i<8; i++){
        coord dst =(coord){src.x + dx[i], src.y + dy[i]};
        if(!checkBound(dst)) continue;
        Piece Targetpiece = chessboard[dst.y][dst.x];
        
        // Block by loyal subject -> can't continue
        if(Targetpiece.type != None && Targetpiece.color == piece.color) continue;

        // Cannot walk into attacked squares
        if((game->turn == White && BlackAttacking[dst.y*8+dst.x]) || (game->turn == Black && WhiteAttacking[dst.y*8+dst.x])) continue;

        // Add move to movelist
            if(moves->count >= moves->capacity) expandMovelist(moves);
            moves->move[moves->count] = (Move){src, dst};
            moves->count++;
    }

    // No castling allowed if in check
    if(game->turn == White && game->WhiteinCheck) return;
    if(game->turn == Black && game->BlackinCheck) return;

    // Castling
    if(piece.color == White){
        // Kingside castling
        if(game->W_k == true){
            bool canCastle = true;
            // Squares between king and rook must be empty and king path must not be attacked: (5,0), (6,0)
            for(int x = 5; x <= 6; x++){
                if(chessboard[0][x].type != None || BlackAttacking[x]){
                    canCastle = false;
                    break;
                }
            }
            
            if(canCastle){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, (coord){6, 0}};
                moves->count++;
            }
        }
        // Queenside castling
        if(game->W_q == true){
            bool canCastle = true;
            // Squares between king and rook ((1,0), (2,0), (3,0)) must be empty and king path ((2,0), (3,0)) must not be attacked
            for(int x = 1; x <= 3; x++){
                if(chessboard[0][x].type != None || (BlackAttacking[x] && x != 1)){
                    canCastle = false;
                    break;
                }
            }

            if(canCastle){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, (coord){2, 0}};
                moves->count++;
            }
        }
    } else {
        // Kingside castling
        if(game->B_k == true){
            bool canCastle = true;
            // Squares between king and rook must be empty and king path must not be attacked: (5,0), (6,0)
            for(int x = 5; x <= 6; x++){
                if(chessboard[7][x].type != None || WhiteAttacking[7*8+x]){
                    canCastle = false;
                    break;
                }
            }
            
            if(canCastle){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, (coord){6, 7}};
                moves->count++;
            }
        }
        // Queenside castling
        if(game->B_q == true){
             bool canCastle = true;
            for(int x = 1; x <= 3; x++){
                if(chessboard[7][x].type != None || (WhiteAttacking[7*8+x] && x != 1)){
                    canCastle = false;
                    break;
                }
            }

            if(canCastle){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, (coord){2, 7}};
                moves->count++;
            }
        }
    }
}

// Generate all moves for knights
static inline void GenerateKnightMoves(Movelist *moves, coord src, Piece piece, Piece chessboard[8][8]){
    // Knight jumps offsets
    int dx[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    int dy[8] = {-1, -2, -2, -1, 1, 2, 2, 1};

    for(int i = 0; i<8; i++){
        coord dst = (coord){src.x + dx[i], src.y+dy[i]};
        if(!checkBound(dst)) continue;
        Piece Targetpiece = chessboard[dst.y][dst.x];

        // Blocked by friendly piece -> can't jump;
        if(Targetpiece.type != None && Targetpiece.color == piece.color) continue;

        // Add move to movelist
        if(moves->count >= moves->capacity) expandMovelist(moves);
        moves->move[moves->count] = (Move){src, dst};
        moves->count++;
    }
}

// Generate all moves for pawns
static inline void GeneratePawnMoves(Movelist *moves, coord src, Piece piece, Piece chessboard[8][8], GameState *game){
    if(piece.color == White){
        // White pawn move foward one square
        coord dst = (coord){src.x, src.y+1};
        Piece Targetpiece = chessboard[dst.y][dst.x];
        // In-bound and no pieces blocking the space
        if(checkBound(dst) && Targetpiece.type == None){
            int End = 1;
            // Add the move pawn move to list 4 times if pawn can be promoted
            if(dst.y == 7) End = 4;
            for(int i = 0; i<End; i++){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, dst};
                moves->count++;
            }
        }
        // Capture pieces diagonal to it + en passant
        for(int i = -1; i<2; i+=2){
            coord diag = (coord){src.x+i, src.y+1};
            Targetpiece = chessboard[diag.y][diag.x];
            // Able to capture the diagonal piece
            if(checkBound(diag) && Targetpiece.type != None && Targetpiece.color != piece.color){
                int End = 1;
                if(diag.y == 7) End = 4;
                for(int i = 0; i<End; i++){
                    if(moves->count >= moves->capacity) expandMovelist(moves);
                    moves->move[moves->count] = (Move){src, diag};
                    moves->count++;
                }
            }
            // check En Passant target square
            if(game->EnPassant.x == diag.x && game->EnPassant.y == diag.y){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, diag};
                moves->count++;
            }
        }
        // Two spaces initial jump
        if(src.y == 1){
            coord jump = (coord){src.x, src.y+2};
            Piece pieceinfront = chessboard[src.y+1][src.x];
            Piece Targetpiece = chessboard[jump.y][jump.x];
            // No pieces blocking the square
            if(Targetpiece.type == None && pieceinfront.type == None){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, jump};
                moves->count++;
            }
        }
    } else{
        // Black pawns move downwards one square
        coord dst = (coord){src.x, src.y-1};
        Piece Targetpiece = chessboard[dst.y][dst.x];
        // In-bound and no pieces blocking the space
        if(checkBound(dst) && Targetpiece.type == None){
            int End = 1;
            // Add the move pawn move to list 4 times if pawn can be promoted
            if(dst.y == 0) End = 4;
            for(int i = 0; i<End; i++){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, dst};
                moves->count++;
            }
        }
        // Capture pieces diagonal to it + en passant
        for(int i = -1; i<2; i+=2){
            coord diag = (coord){src.x + i, src.y - 1};
            Targetpiece = chessboard[diag.y][diag.x];
            // Able to capture the diagonal piece
            if(checkBound(diag) && Targetpiece.type != None && Targetpiece.color != piece.color){
                int End = 1;
                if(diag.y == 0) End = 4;
                for(int i = 0; i<End; i++){
                    if(moves->count >= moves->capacity) expandMovelist(moves);
                    moves->move[moves->count] = (Move){src, diag};
                    moves->count++;
                }
            }
            // check En Passant target square
            if(game->EnPassant.x == diag.x && game->EnPassant.y == diag.y){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, diag};
                moves->count++;
            }
        }
        // Two spaces initial jump
        if(src.y == 6){
            coord jump = (coord){src.x, src.y-2};
            Piece pieceinfront = chessboard[src.y-1][src.x];
            Piece Targetpiece = chessboard[jump.y][jump.x];
            // No pieces blocking the square
            if(Targetpiece.type == None && pieceinfront.type == None){
                if(moves->count >= moves->capacity) expandMovelist(moves);
                moves->move[moves->count] = (Move){src, jump};
                moves->count++;
            }
        }
    }
}

static inline void FilterLegalMoves(Movelist* pseudoMoves, GameState * game, const coord MrPresidentSquares[16], const int MrPresidentcount){

    // List to store legal moves
    Movelist legalMoves;
    legalMoves.capacity = 32;
    legalMoves.count = 0;
    legalMoves.move = (Move*)malloc(legalMoves.capacity * sizeof(Move));

    bool *Move_isLegal;
    Move_isLegal = (bool* )malloc(pseudoMoves->count*sizeof(bool));
    for(int i = 0; i < pseudoMoves->count; i++) Move_isLegal[i] = true; // All moves are innocent until proven otherwise

    // Inititial checks filter
    for(int moveIdx = 0; moveIdx < pseudoMoves->count; moveIdx++){
        Move move = pseudoMoves->move[moveIdx];
        // The King cannot cover himself
        if(move.src.y*8+move.src.x == game->BlackKingIdx || move.src.y*8+move.src.x == game->WhiteKingIdx) continue;
        // If is check -> resolve check by shielding the king or capturing the target
        if(game->turn == White && game->WhiteinCheck){
            bool found = false;
            for(int humanshield = 0; humanshield < MrPresidentcount; humanshield++){
                if(Coordscmp(move.dst, MrPresidentSquares[humanshield])){
                    // Found a legal move -> mark as true and search next move
                    found = true;
                    break;
                } 
            }
            if(!found)Move_isLegal[moveIdx] = false;
        } else if(game->turn == Black && game->BlackinCheck){
            bool found = false;
            for(int humanshield = 0; humanshield < MrPresidentcount; humanshield++){
                if(Coordscmp(move.dst, MrPresidentSquares[humanshield])){
                    // Found a legal move -> mark as true and search next move
                    found = true;
                    break;
                }
            }
            if(!found)Move_isLegal[moveIdx] = false;
        }
        // When not in check -> most moves are legal
    }

    // Restrict pinned pieces
    for(int moveIdx = 0; moveIdx < pseudoMoves->count; moveIdx++){
        if(!Move_isLegal[moveIdx]) continue;
        Move move = pseudoMoves->move[moveIdx];
        if(game->turn == White){   
            for(int pin = 0; pin < game->pinnedWcount; pin++){
                if(Coordscmp(move.src, game->pWPieces[pin].src)){
                    bool foundLegal = false;
                    for(int pinnedmove = 0; pinnedmove < game->pWPieces[pin].movecount; pinnedmove++){
                        // Check along the dir to find legal move
                        if(Coordscmp(move.dst, game->pWPieces[pin].dst[pinnedmove])){
                            foundLegal = true;
                            break;
                        }
                    }
                    // Only allow moves that match the allowed pin direction
                    if(!foundLegal) Move_isLegal[moveIdx] = false;
                    break;
                }
            }
        } else if(game->turn == Black){   
            for(int pin = 0; pin < game->pinnedBcount; pin++){
                if(Coordscmp(move.src, game->pBPieces[pin].src)){
                    bool foundLegal = false;
                    for(int pinnedmove = 0; pinnedmove < game->pBPieces[pin].movecount; pinnedmove++){
                        if(Coordscmp(move.dst, game->pBPieces[pin].dst[pinnedmove])){
                            foundLegal = true;
                            break;
                        }
                    }
                    if(!foundLegal) Move_isLegal[moveIdx] = false;
                    break;
                }
            }
        }
    }

    // Filter legal king moves
    for(int moveIdx = 0; moveIdx < pseudoMoves->count; moveIdx++){
        if(!Move_isLegal[moveIdx]) continue;
        if(game->turn == White){
            Move move = pseudoMoves->move[moveIdx];
            int KingIdx = move.src.y*8+move.src.x;
            // Skip if piece is not the king
            if(KingIdx != game->WhiteKingIdx) continue;
            int targetIdx = move.dst.y*8+move.dst.x;
            // Mark the move as false if square is under attack
            if(BlackAttacking[targetIdx]) Move_isLegal[moveIdx] = false;
        } else if(game->turn == Black){
            Move move = pseudoMoves->move[moveIdx];
            int KingIdx = move.src.y*8+move.src.x;
            // Skip if piece is not the king
            if(KingIdx != game->BlackKingIdx) continue;
            int targetIdx = move.dst.y*8+move.dst.x;
            // Mark the move as false if square is under attack
            if(WhiteAttacking[targetIdx]) Move_isLegal[moveIdx] = false;
        }
    }

    // Admit the new legal moves
    for(int moveIdx = 0; moveIdx < pseudoMoves->count; moveIdx++){
        if(Move_isLegal[moveIdx]){
            if(legalMoves.capacity <= legalMoves.count) expandMovelist(&legalMoves);
            legalMoves.move[legalMoves.count] = pseudoMoves->move[moveIdx];
            legalMoves.count++;
        }
    }

    // Free legality reference
    free(Move_isLegal);

    // Swap the move pointer and update count/capacity
    Move* oldMoveArray = pseudoMoves->move;
    pseudoMoves->move = legalMoves.move;
    pseudoMoves->count = legalMoves.count;
    pseudoMoves->capacity = legalMoves.capacity;

    // Free the old move array
    if (oldMoveArray != NULL) {
        free(oldMoveArray);
    }
}

// Generate a list of all possible moves in the current position
void GenerateMoveList(Movelist *moves, Piece chessboard[8][8], coord PieceLocations[32], GameState *game){
    // Movelist init
    if(moves->move == NULL) moves->move = (Move *) malloc(moves->capacity*sizeof(Move));
    moves->count = 0;

    int assassins = 0;
    int MrPresidentcount = 0;
    coord MrPresidentSquares[16]; // Pieces must jump into these squares to block a check
    for(int i = 0; i < 16; i++) MrPresidentSquares[i] = (coord){8, 8};
    findAssassins(chessboard, game, &assassins, MrPresidentSquares, &MrPresidentcount);

    //printf("Assassins: %d\n", assassins);
    // for(int i = 0; i<MrPresidentcount; i++){
    //     printf("MrPresident: %d %d\n", MrPresidentSquares[i].x, MrPresidentSquares[i].y);
    // }

    // If double checked, only generate king moves
    if(assassins > 1){
        for(int i = 0; i < 32; i++){
            if(PieceLocations[i].x < 8 && PieceLocations[i].y < 8){
                coord src = PieceLocations[i];
                Piece piece = chessboard[src.y][src.x];
                if(piece.color == game->turn && piece.type == King){
                    GenerateKingMoves(moves, src, piece, chessboard, game);
                    break; // Only one king per side
                }
            }
        }
    } else {// Perform move generation for all piece types at the right turn
        for(int i = 0; i < 32; i++){
            if(PieceLocations[i].x < 8 && PieceLocations[i].y < 8){
                coord src = PieceLocations[i];
                Piece piece = chessboard[src.y][src.x];
                if(piece.color == game->turn){
                    if(piece.type == Bishop || piece.type == Rook || piece.type == Queen){
                        GenerateSlidingMoves(moves, src, piece, chessboard);
                    } else if(piece.type == King){
                        GenerateKingMoves(moves, src, piece, chessboard, game);
                    } else if(piece.type == Knight){
                        GenerateKnightMoves(moves, src, piece, chessboard);
                    } else if(piece.type == Pawn){
                        GeneratePawnMoves(moves, src, piece, chessboard, game);
                    }
                }
            }
        }
    }

    FilterLegalMoves(moves, game, MrPresidentSquares, MrPresidentcount);
}

void recordMove(UnmakeInfo** unmakeStack, const Move move, const Piece piece, const Piece Targetpiece, GameState* game, const char PawnPromote){
    UnmakeInfo *nextunmake = (UnmakeInfo* )malloc(sizeof(UnmakeInfo));
    nextunmake->prevPiece = piece;
    nextunmake->capturedPiece = Targetpiece;
    // Keep track if the previous move was a promotion
    nextunmake->Promoted = (PawnPromote != '\0')? true:false;
    nextunmake->prevGame = *game;
    nextunmake->prevMove = move;
    *unmakeStack = push(nextunmake, *unmakeStack);
}

// make a move that has already been validated
void makeMove(Piece chessboard[8][8], coord PieceLocations[32], Move move, GameState *game, const char PawnPromote, UnmakeInfo** unmakeStack){

    Piece piece = chessboard[move.src.y][move.src.x];
    Piece Targetpiece = chessboard[move.dst.y][move.dst.x];
    recordMove(unmakeStack, move, piece, Targetpiece, game, PawnPromote);

    // update King's location
    if(piece.type == King){
        if(piece.color == White) game->WhiteKingIdx = move.dst.y*8+move.dst.x;
        else game->BlackKingIdx = move.dst.y*8+move.dst.x;
    }

    // Movement and capture handling
    for(int i = 0; i<32; i++){
        if(Coordscmp(PieceLocations[i], move.src)){
            if(chessboard[move.dst.y][move.dst.x].type != None){
                for(int j = 0; j<32; j++){
                    if(Coordscmp(PieceLocations[j], move.dst)){
                        PieceLocations[j] = (coord){8, 8};
                    }
                }
            }
            PieceLocations[i] = move.dst;
        }
    }
    chessboard[move.src.y][move.src.x].type = None;
    chessboard[move.dst.y][move.dst.x] = piece;

    // Pawn promotion
    if(piece.type == Pawn && PawnPromote != '\0'){
        switch(PawnPromote){
            case 'b': chessboard[move.dst.y][move.dst.x].type = Bishop; break;
            case 'n': chessboard[move.dst.y][move.dst.x].type = Knight; break;
            case 'r': chessboard[move.dst.y][move.dst.x].type = Rook; break;
            case 'q': chessboard[move.dst.y][move.dst.x].type = Queen; break;
        }
    }

    // En Passant capture
    if(piece.type == Pawn && Coordscmp(game->EnPassant, move.dst)){
        coord dst = (coord){move.dst.x, move.dst.y};
        dst.y += (piece.color == White)? -1:1;
        for(int i = 0; i<32; i++){
                // Piece no longer in play
                if(Coordscmp(dst, PieceLocations[i])) PieceLocations[i] = (coord){8, 8};
            }
        chessboard[dst.y][dst.x].type = None;
    }

    //En Passant target square update
    if(piece.type == Pawn && abs(move.dst.y - move.src.y) == 2){
        if(piece.color == White) game->EnPassant = (coord){move.dst.x, move.dst.y-1};
        else game->EnPassant = (coord){move.dst.x, move.dst.y+1};
    } else game->EnPassant = (coord){8, 8};

    // Castling
    if(piece.type == King && abs(move.dst.x - move.src.x) > 1){
        // Castling on which side
        if(move.dst.x == 2){ // Queen side
            if(piece.color == White && game->W_q){
                // update rook position
                coord RookLocation = (coord){0, 0};
                coord newRookLocation = (coord){3, 0};
                for(int i = 0; i<32; i++){
                    if(Coordscmp(PieceLocations[i], RookLocation))
                    {
                        PieceLocations[i] = newRookLocation;
                        chessboard[RookLocation.y][RookLocation.x].type = None;
                        chessboard[newRookLocation.y][newRookLocation.x].type = Rook;
                        chessboard[newRookLocation.y][newRookLocation.x].color = White;
                    }
                }
            } else if(piece.color == Black && game->B_q){
                // update rook position
                coord RookLocation = (coord){0, 7};
                coord newRookLocation = (coord){3, 7};
                for(int i = 0; i<32; i++){
                    if(Coordscmp(PieceLocations[i], RookLocation))
                    {
                        PieceLocations[i] = newRookLocation;
                        chessboard[RookLocation.y][RookLocation.x].type = None;
                        chessboard[newRookLocation.y][newRookLocation.x].type = Rook;
                        chessboard[newRookLocation.y][newRookLocation.x].color = Black;
                    }
                }
            }
        } else if(move.dst.x == 6){ // King side
            if(piece.color == White && game->W_k){
                // update rook position
                coord RookLocation = (coord){7, 0};
                coord newRookLocation = (coord){5, 0};
                for(int i = 0; i<32; i++){
                    if(Coordscmp(PieceLocations[i], RookLocation))
                    {
                        PieceLocations[i] = newRookLocation;
                        chessboard[RookLocation.y][RookLocation.x].type = None;
                        chessboard[newRookLocation.y][newRookLocation.x].type = Rook;
                        chessboard[newRookLocation.y][newRookLocation.x].color = White;
                    }
                }
            } else if(piece.color == Black && game->B_k){
                // update rook position
                coord RookLocation = (coord){7, 7};
                coord newRookLocation = (coord){5, 7};
                for(int i = 0; i<32; i++){
                    if(Coordscmp(PieceLocations[i], RookLocation))
                    {
                        PieceLocations[i] = newRookLocation;
                        chessboard[RookLocation.y][RookLocation.x].type = None;
                        chessboard[newRookLocation.y][newRookLocation.x].type = Rook;
                        chessboard[newRookLocation.y][newRookLocation.x].color = Black;
                    }
                }
            }
        }
    }

    // Castling rights update
    if(piece.color == White && (game->W_k || game->W_q)){
        // King moved -> all castling rights loss
        if(piece.type == King){
            game->W_k = false;
            game->W_q = false;
        }
        // Rook moved -> castling on that side is loss
        if(piece.type == Rook){
            if(move.src.x == 0) game->W_q = false; // Queen side rook
            else if(move.src.x == 7) game->W_k = false; // King side rook
        }
        // Captured enemy rook -> enemy loses castling rights
        if(Targetpiece.type == Rook && (game->B_k || game->B_q)){
            if(move.dst.x == 0) game->B_q = false; // Queen side rook
            else if(move.dst.x == 7) game->B_k = false; // King side rook
        }
    } else if(piece.color == Black && (game->B_k || game->B_q)){
    // King moved -> all castling rights loss
        if(piece.type == King){
            game->B_k = false;
            game->B_q = false;
        }
        // Rook moved -> castling on that side is loss
        if(piece.type == Rook){
            if(move.src.x == 0) game->B_q = false; // Queen side rook
            else if(move.src.x == 7) game->B_k = false; // King side rook
        }
        // Captured enemy rook -> enemy loses castling rights
        if(Targetpiece.type == Rook && (game->W_k || game->W_q)){
            if(move.dst.x == 0) game->W_q = false; // Queen side rook
            else if(move.dst.x == 7) game->W_k = false; // King side rook
        }
    }

    // Update halfmove clock
    game->HalfmoveClock++;
    if(piece.type == Pawn || Targetpiece.type != None){
        game->HalfmoveClock = 0;
    }
    // Update fullmove number
    if(game->turn == game->firstToMove) game->FullmoveNum++;

    // Switch turn
    game->turn = (game->turn == White)? Black:White;

    // Update check and pin state for the new side to move
    checkforChecks(chessboard, PieceLocations, game);
    updatePinnedPieces(chessboard, game);
}

void unmakeMove(UnmakeInfo** unmakeStack, GameState *game, Piece chessboard[8][8], coord PieceLocations[32]){
    Piece piece = (*unmakeStack)->prevPiece;
    Piece Targetpiece = (*unmakeStack)->capturedPiece;
    coord src = (*unmakeStack)->prevMove.src;
    coord dst = (*unmakeStack)->prevMove.dst;

    // revert all gamestate statistics
    *game = (*unmakeStack)->prevGame;

    // Revert the king's position
    if(piece.type == King){
        if(piece.color == White) game->WhiteKingIdx = src.y*8+src.x;
        else game->BlackKingIdx = src.y*8+src.x;
    }

    // Revert Pawn promotions
    if((*unmakeStack)->Promoted) piece.type = Pawn;

    // Revert the positions of the pieces which were in play
    for(int i = 0; i<32; i++){
        if(Coordscmp(PieceLocations[i], dst)){
            // Put the piece back to the previous square
            PieceLocations[i] = src;
        }
    }
    // Restore any piece that was taken
    if(Targetpiece.type != None){
        for(int j = 0; j<32; j++) if(Coordscmp(PieceLocations[j], (coord){8, 8})){
            PieceLocations[j] = dst;
            break;
        }
    }
    chessboard[src.y][src.x] = piece;
    chessboard[dst.y][dst.x] = Targetpiece;

    // Revert captured En Passant pawn
    if(piece.type == Pawn && Coordscmp(dst, (*unmakeStack)->prevGame.EnPassant)){
        coord oldpos = dst;
        oldpos.y += (piece.color == White)? -1:1;
        for(int i = 0; i<32; i++){
            if(Coordscmp(PieceLocations[i], (coord){8, 8})) PieceLocations[i] = oldpos;
            break;
        }
        chessboard[oldpos.y][oldpos.x].type = Pawn;
        chessboard[oldpos.y][oldpos.x].color = (piece.type == White)? White:Black;
    }

    // Revert Rooks' locations after castling
    if(piece.type == King && abs(src.x - dst.x) > 1){
        if(dst.x == 2){ // Queen side
            coord rookpos = (piece.color == White)? (coord){3, 0} : (coord){3, 7};
            coord oldrookpos = (piece.color == White)? (coord){0, 0} : (coord){0, 7};
            for(int i = 0; i<32; i++){
                if(Coordscmp(PieceLocations[i], rookpos)){
                    PieceLocations[i] = oldrookpos;
                    chessboard[rookpos.y][rookpos.x].type = None;
                    chessboard[oldrookpos.y][oldrookpos.x].type = Rook;
                    chessboard[oldrookpos.y][oldrookpos.x].color = (piece.color == White)? White:Black;
                }
            }
        } else if(dst.x == 6){ // King side
            coord rookpos = (piece.color == White)? (coord){5, 0} : (coord){5, 7};
            coord oldrookpos = (piece.color == White)? (coord){7, 0} : (coord){7, 7};
            for(int i = 0; i<32; i++){
                if(Coordscmp(PieceLocations[i], rookpos)){
                    PieceLocations[i] = oldrookpos;
                    chessboard[rookpos.y][rookpos.x].type = None;
                    chessboard[oldrookpos.y][oldrookpos.x].type = Rook;
                    chessboard[oldrookpos.y][oldrookpos.x].color = (piece.color == White)? White:Black;
                }
            }
        }
    }

    // remove from list of previous moves
    *unmakeStack = pop(*unmakeStack);
    
    // recalculate checks and pins
    checkforChecks(chessboard, PieceLocations, game);
    updatePinnedPieces(chessboard, game);
}

long long perftTest(Piece chessboard[8][8], coord PieceLocations[32], GameState *game, UnmakeInfo** unmakeStack, const int depth){
    // return condition
    if(depth == 0) return 1;

    // Make a new list of moves for each recursion
    Movelist moves;
    moves.capacity = 32;
    moves.count = 0;
    moves.move = NULL;

    GenerateMoveList(&moves, chessboard, PieceLocations, game);

    long long nodes = 0;

    // Recursively count the number of positions possible
    for(int i = 0; i<moves.count; i++){
        Piece piece = chessboard[moves.move[i].src.y][moves.move[i].src.x];
        char PawnPromote = '\0';
        // Handle the four possible promotion paths for pawns
        if(piece.type == Pawn && ((piece.color == White && moves.move[i].dst.y == 7) || (piece.color == Black && moves.move[i].dst.y == 0))){
            int promoteIdx = 0;
            for(int j=i-1; j>=0; j--){
                if  (Coordscmp(moves.move[i].src, moves.move[j].src)
                    && Coordscmp(moves.move[i].dst, moves.move[j].dst))
                    promoteIdx++;
                else break;
            }
            switch(promoteIdx){
                case 0: PawnPromote = 'b'; break;
                case 1: PawnPromote = 'n'; break;
                case 2: PawnPromote = 'r'; break;
                case 3: PawnPromote = 'q'; break;
            }
        }
        makeMove(chessboard, PieceLocations, moves.move[i], game, PawnPromote, unmakeStack);
        nodes += perftTest(chessboard, PieceLocations, game, unmakeStack, depth - 1);
        unmakeMove(unmakeStack, game, chessboard, PieceLocations);
    }

    free(moves.move);

    return nodes;
}

