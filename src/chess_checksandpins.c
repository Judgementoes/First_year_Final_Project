#include "chess_checksandpins.h"

static inline void MarkMrPresidentSquares(const enum PieceType type, coord MrPresidentSquares[16], int *MrPresidentcount, const int KingSquare, const int EnemySquare, const int dirIdx){
    if(type == Rook || type == Bishop || type == Queen){ // sliding pieces
        int slidingOffset[8] = {8, -8, 1, -1, 7, 9, -7, -9};
        int targetSquare = EnemySquare;
        while(targetSquare != KingSquare){
            MrPresidentSquares[(*MrPresidentcount)++] = (coord){targetSquare%8, targetSquare/8};
            targetSquare += slidingOffset[dirIdx];
        }
    }else {
        MrPresidentSquares[(*MrPresidentcount)++] = (coord){EnemySquare%8, EnemySquare/8};
    }
}

static inline void MarkSlidingMoves(const Piece piece, const coord src, const Piece chessboard[8][8]){
    // Eight directional offset for 1D board
    int offset[8] = {-8, 8, -1, 1, -7, -9, 7, 9};

    // Logic to filter bishops, rooks and queens
    int Start = (piece.type == Bishop)? 4:0;
    int End = (piece.type == Rook)? 4:8;

    const int SquareIdx = src.y*8 + src.x;
    for(int dirIdx = Start; dirIdx < End; dirIdx++){
        for(int traverseIdx = 0; traverseIdx < NumSquarestoEdge[SquareIdx][dirIdx]; traverseIdx++){
            int TargetSquare = SquareIdx + offset[dirIdx]*(traverseIdx + 1);

            // Translate 1D index to 2D coords -> lookup piece on chessboard
            coord dst = (coord){TargetSquare%8, TargetSquare/8};
            Piece Targetpiece = chessboard[dst.y][dst.x];

            // Mark the square as under attacked
            if(piece.color == Black) BlackAttacking[TargetSquare] = true;
            else if(piece.color == White) WhiteAttacking[TargetSquare] = true;

            // Blocked by piece -> can't continue (friendly piece is treated as protected -> King cannot take)
            if(Targetpiece.type != None && !(Targetpiece.type == King && Targetpiece.color != piece.color)) break;

        }
    }
}

static inline void MarkBasicMoves(const Piece piece, const coord src, const Piece chessboard[8][8]){
    // These pieces uses the same logic to mark attacking squares
    int *offset_x, *offset_y;
    int knightoffset_x[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    int knightoffset_y[8] = {-1, -2, -2, -1, 1, 2, 2, 1};
    int kingoffset_x[8] = {0, 0, -1, 1, 1, -1, -1, 1}; // To enforce that 2 kings cannot stand next to each other
    int kingoffset_y[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
    if(piece.type == Knight) {
        offset_x = knightoffset_x;
        offset_y = knightoffset_y;
    }
    else if(piece.type == King){
        offset_x = kingoffset_x;
        offset_y = kingoffset_y;
    }
    else if(piece.type == Pawn){
        // Relative directions for pawns (does not affect Kings and Knights)
        int relativeDir = (piece.color == White)? 1:-1;
        for(int diag = -1; diag < 2; diag+=2){
            coord dst = (coord){src.x + diag, src.y + relativeDir};
            // skip if out of bounds
            if(!checkBound(dst)) continue;
            
            int targetSquare = dst.y*8+dst.x;

            if(piece.color == Black) BlackAttacking[targetSquare] = true;
            else if(piece.color == White) WhiteAttacking[targetSquare] = true;
        }
        return;
    }

    const int SquareIdx = src.y*8 + src.x;
    for(int dirIdx = 0; dirIdx < 8; dirIdx++){

        coord dst = src;
        dst.x += offset_x[dirIdx];
        dst.y += offset_y[dirIdx];

        // Move is not within bounds -> skip
        if(!checkBound(dst)) continue;

        int targetSquare = dst.y*8+dst.x;

        // Mark all the squares as under attack (Friendly pieces are treated as protected)
        if(piece.color == Black) BlackAttacking[targetSquare] = true;
        else if(piece.color == White) WhiteAttacking[targetSquare] = true;
    }
}

static inline void updateAttackedSquares(Piece chessboard[8][8], const coord PieceLocations[32], const GameState *game){
    // re-init both arrays
    for(int i = 0; i < 64; i++){
        BlackAttacking[i] = false;
        WhiteAttacking[i] = false;
    }

    // Mark the squares which are under attack
    for(int i = 0; i < 32; i++){
        coord src = PieceLocations[i];
        if(src.x < 8 && src.y < 8){
            Piece piece = chessboard[src.y][src.x];
            if(piece.color != game->turn){ // No need to update the attacking squares of pieces of the playing side
                if(piece.type == Bishop || piece.type == Rook || piece.type == Queen){  // Sliding pieces
                    MarkSlidingMoves(piece, src, chessboard);
                } else MarkBasicMoves(piece, src, chessboard); // Other pieces
            }
        }
    }
}

/** This function both check for checks and update attacked squares **/
void checkforChecks(Piece chessboard[8][8], coord PieceLocations[32], GameState *game){ // Tears For Fears ahh name
    // get data on squares being attack
    updateAttackedSquares(chessboard, PieceLocations, game);

    game->WhiteinCheck = false;
    game->BlackinCheck = false;

    // Check for checks for the side whose turn it is to move
    if(game->turn == White){
        if(BlackAttacking[game->WhiteKingIdx]){ // White king in check
            game->WhiteinCheck = true;
        } else game->WhiteinCheck = false;
    } else if(game->turn == Black){
        if(WhiteAttacking[game->BlackKingIdx]){ // Black king in check
            game->BlackinCheck = true;
        } else game->BlackinCheck = false;
    }
}

void findAssassins(const Piece chessboard[8][8], const GameState *game, int *assassins, coord MrPresidentSquares[16], int *MrPresidentcount){
    if(((game->turn == White && !game->WhiteinCheck) || (game->turn == Black && !game->BlackinCheck))){
        *assassins = 0;
        return;
    }
    int slidingOffset[8] = {-8, 8, -1, 1, -7, -9, 7, 9};
    int knightOffset[8] = {-6, -15, -17, -10, 6, 15, 17, 10};    
    for(int i = 0; i<16; i++) MrPresidentSquares[i] = (coord){8, 8};
    if(game->turn == White && game->WhiteinCheck){

        // Check sliding pieces directions (including pawns)
        for(int dirIdx = 0; dirIdx < 8; dirIdx++){

            // King can not be checked more than 2 times
            if(*assassins == 2) break;

            for(int traverseIdx = 0; traverseIdx < NumSquarestoEdge[game->WhiteKingIdx][dirIdx]; traverseIdx++){
                int targetIdx = game->WhiteKingIdx + slidingOffset[dirIdx] * (traverseIdx+1);
                Piece targetPiece = chessboard[targetIdx/8][targetIdx%8];

                // Encounter loyal subject -> safe direction
                if(targetPiece.type != None && targetPiece.color == White) break;

                // Encounter enemy piece -> check if enemy can attack the king
                int Start = 0, End = 8; // Logic to separate pieces
                if (targetPiece.type == Rook) {
                    End = 4;
                    // Encountered enemy rook on the vertical or horizontal
                    if (dirIdx >= Start && dirIdx < End) {
                        MarkMrPresidentSquares(Rook, MrPresidentSquares, MrPresidentcount, game->WhiteKingIdx, targetIdx, dirIdx);
                        (*assassins)++;
                        break;
                    }
                } else if (targetPiece.type == Bishop) {
                    Start = 4;
                    // Encountered enemy bishop on the diagonal
                    if (dirIdx >= Start && dirIdx < End) {
                        MarkMrPresidentSquares(Bishop, MrPresidentSquares, MrPresidentcount, game->WhiteKingIdx, targetIdx, dirIdx);
                        (*assassins)++;
                        break;
                    }
                } else if (targetPiece.type == Queen) {
                    // Queens can check in all directions
                    MarkMrPresidentSquares(Queen, MrPresidentSquares, MrPresidentcount, game->WhiteKingIdx, targetIdx, dirIdx);
                    (*assassins)++;
                    break;
                } else if (targetPiece.type == Pawn) {
                    // Black pawns move up the board
                    Start = 6;
                    End = 8;
                    // Encountered enemy pawn right on the adjacent diagonal square
                    if (dirIdx >= Start && dirIdx < End && traverseIdx == 0) {
                        MarkMrPresidentSquares(Pawn, MrPresidentSquares, MrPresidentcount, game->WhiteKingIdx, targetIdx, dirIdx);
                        (*assassins)++;
                        break;
                    }
                }
            }
        }

        // check possible knight attack angles
        for(int dirIdx = 0; dirIdx < 8; dirIdx++){
            // already in double check -> no need to search
            if(*assassins == 2) break;
            int targetSquare = game->WhiteKingIdx + knightOffset[dirIdx];
            coord dst = (coord){targetSquare%8, targetSquare/8};
            
            // Check if the horse is on board
            if(!checkBound(dst)) continue;

            Piece targetPiece = chessboard[targetSquare/8][targetSquare%8];

            // Petah, the horse is here
            if(targetPiece.type == Knight && targetPiece.color != game->turn){
                MarkMrPresidentSquares(Knight, MrPresidentSquares, MrPresidentcount, game->WhiteKingIdx, targetSquare, dirIdx);
                (*assassins)++;
            }
        }
    } else if(game->turn == Black && game->BlackinCheck){

        // Check sliding pieces directions (including pawns)
        for(int dirIdx = 0; dirIdx < 8; dirIdx++){

            // King can not be checked more than 2 times
            if(*assassins == 2) break;

            for(int traverseIdx = 0; traverseIdx < NumSquarestoEdge[game->BlackKingIdx][dirIdx]; traverseIdx++){
                int targetIdx = game->BlackKingIdx + slidingOffset[dirIdx] * (traverseIdx+1);
                Piece targetPiece = chessboard[targetIdx/8][targetIdx%8];

                // Encounter loyal subject -> safe direction
                if(targetPiece.type != None && targetPiece.color == Black) break;

                // Encounter enemy piece -> check if enemy can attack the king
                int Start = 0, End = 8; // Logic to separate pieces
                if (targetPiece.type == Rook) {
                    End = 4;
                    // Encountered enemy rook on the vertical or horizontal
                    if (dirIdx >= Start && dirIdx < End) {
                        MarkMrPresidentSquares(Rook, MrPresidentSquares, MrPresidentcount, game->BlackKingIdx, targetIdx, dirIdx);
                        (*assassins)++;
                        break;
                    }
                } else if (targetPiece.type == Bishop) {
                    Start = 4;
                    // Encountered enemy bishop on the diagonal
                    if (dirIdx >= Start && dirIdx < End) {
                        MarkMrPresidentSquares(Bishop, MrPresidentSquares, MrPresidentcount, game->BlackKingIdx, targetIdx, dirIdx);
                        (*assassins)++;
                        break;
                    }
                } else if (targetPiece.type == Queen) {
                    // Queens can check in all directions
                    if(targetPiece.color == White) {
                        MarkMrPresidentSquares(Queen, MrPresidentSquares, MrPresidentcount, game->BlackKingIdx, targetIdx, dirIdx);
                        (*assassins)++;
                        break;
                    }
                } else if (targetPiece.type == Pawn) {
                    // White pawns move down the board
                    Start = 4;
                    End = 6;
                    // Encountered enemy pawn right on the adjacent diagonal square
                    if (dirIdx >= Start && dirIdx < End && traverseIdx == 0) {
                        MarkMrPresidentSquares(Pawn, MrPresidentSquares, MrPresidentcount, game->BlackKingIdx, targetIdx, dirIdx);
                        (*assassins)++;
                        break;
                    }
                }
            }
        }

        // check possible knight attack angles
        for(int dirIdx = 0; dirIdx < 8; dirIdx++){
            // already in double check -> no need to search
            if(*assassins == 2) break;
            int targetSquare = game->BlackKingIdx + knightOffset[dirIdx];

            coord dst = (coord){targetSquare%8, targetSquare/8};
            
            // Check if the horse is on board
            if(!checkBound(dst)) continue;

            Piece targetPiece = chessboard[targetSquare/8][targetSquare%8];

            // Petah, the horse is here
            if(targetPiece.type == Knight && targetPiece.color == White){
                MarkMrPresidentSquares(Knight, MrPresidentSquares, MrPresidentcount, game->BlackKingIdx, targetSquare, dirIdx);
                (*assassins)++;
            }
        }
    }
}

static inline void MarkPinnedMoves(GameState *game, const coord src, const int kingSquare, const int enemySquare, const int dirIdx){
    int offset[8] = {8, -8, 1, -1, 7, 9, -7, -9}; 
    if(game->turn == White){
        int idx = game->pinnedWcount;
        game->pinnedWcount++;
        game->pWPieces[idx].src = src;
        game->pWPieces[idx].movecount = 0;
        int targetSquare = enemySquare;
        while(targetSquare != kingSquare){
            game->pWPieces[idx].dst[game->pWPieces[idx].movecount++] = (coord){targetSquare%8, targetSquare/8};
            // printf("src: %d %d\ndst #%d: %d %d\n", src.x, src.y, game->pWPieces[idx].movecount, game->pWPieces[idx].dst[game->pWPieces[idx].movecount-1].x, game->pWPieces[idx].dst[game->pWPieces[idx].movecount-1].y);
            targetSquare += offset[dirIdx];
        }
    } else if(game->turn == Black){
        int idx = game->pinnedBcount;
        game->pinnedBcount++;
        game->pBPieces[idx].src = src;
        game->pBPieces[idx].movecount = 0;
        int targetSquare = enemySquare;
        while(targetSquare != kingSquare){
            game->pBPieces[idx].dst[game->pBPieces[idx].movecount++] = (coord){targetSquare%8, targetSquare/8};
            // printf("src: %d %d\ndst #%d: %d %d\n", src.x, src.y, game->pBPieces[idx].movecount, game->pBPieces[idx].dst[game->pBPieces[idx].movecount-1].x, game->pBPieces[idx].dst[game->pBPieces[idx].movecount-1].y);
            targetSquare += offset[dirIdx];
        }
    }
}

void updatePinnedPieces(const Piece chessboard[8][8], GameState *game){
    // reset pin counter
    game->pinnedBcount = 0;
    game->pinnedWcount = 0;

    int kingSquare = (game->turn == White)? game->WhiteKingIdx:game->BlackKingIdx;
    //Special en passant case
    int Pawnoffset[2] = {-1, 1}; // Check to the left and right of the kings
    // if White king is on the 5th rank or Black king is on the 4th rank
    if((game->turn == White && game->WhiteKingIdx/8 == 4) || (game->turn == Black && game->BlackKingIdx/8 == 3)){
        for(int dirIdx = 2; dirIdx < 4; dirIdx++){
            if(NumSquarestoEdge[kingSquare][dirIdx] < 3) continue; // need at least three squares from the edge of the board
            int EnemypawnCount = 0;
            bool EnPassantable = false;
            for(int traverseIdx = 0; traverseIdx < NumSquarestoEdge[kingSquare][dirIdx]; traverseIdx++){
                int targetSquare = kingSquare + Pawnoffset[dirIdx-2] * (traverseIdx + 1);
                Piece targetPiece = chessboard[targetSquare/8][targetSquare%8];

                // Encounter non-pawn piece -> no need to check further
                if(targetPiece.type != None && targetPiece.type != Pawn && !EnPassantable) break;

                // Encounter enemy pawn -> no need to check anymore if more than 2 + catch the situation where the enemy pawn is further along
                if(targetPiece.type == Pawn && targetPiece.color != game->turn){
                    EnemypawnCount++;
                    if(EnemypawnCount>1) break;
                    continue;
                }

                // When en passant is possible, if a piece is not an enemy rook or queen -> skip
                if(EnPassantable){
                    if(targetPiece.color != game->turn && (targetPiece.type == Rook || targetPiece.type == Queen)){
                       game->EnPassant = (coord){8, 8}; // Disable en passant if true
                        break;
                    } else if(targetPiece.type != None){
                        break;
                    }
                }

                // Encounter friendly pawn -> check if En Passant is possible
                if(targetPiece.type == Pawn && targetPiece.color == game->turn){
                    if(game->turn == White){
                        // White pawns capture downwards
                        coord r = (coord){(targetSquare+9)%8, (targetSquare+9)/8};
                        coord l = (coord){(targetSquare+7)%8, (targetSquare+7)/8};
                        if(Coordscmp(r, game->EnPassant) || (Coordscmp(l, game->EnPassant))) EnPassantable = true; 
                        else break; // Unable to En Passant -> no need to check further
                    } else if(game->turn == Black){
                        // Black pawns capture upwards
                        coord r = (coord){(targetSquare-7)%8, (targetSquare-7)/8};
                        coord l = (coord){(targetSquare-9)%8, (targetSquare-9)/8};
                        if(Coordscmp(r, game->EnPassant) || (Coordscmp(l, game->EnPassant))) EnPassantable = true;
                        else break;
                    }
                }
            }
        }
    }
    // Regular pin logic
    int offset[8] = {-8, 8, -1, 1, -7, -9, 7, 9}; // Only enemy sliders can pin
    for(int dirIdx = 0; dirIdx < 8; dirIdx++){
        int loyalSubjects = 0;
        coord possiblePin;
        for(int traverseIdx = 0; traverseIdx < NumSquarestoEdge[kingSquare][dirIdx]; traverseIdx++){
            int targetSquare = kingSquare + offset[dirIdx] * (traverseIdx + 1);
            Piece targetPiece = chessboard[targetSquare/8][targetSquare%8];

            if(targetPiece.type == None) continue;

            // encounter loyal subject -> save location
            if(targetPiece.color == game->turn){
                loyalSubjects++;
                if(loyalSubjects > 1) break;
                possiblePin = (coord){targetSquare%8, targetSquare/8};
                continue;
            }

            // If enemy piece is seen right after friendly piece -> check whether it xrays the king
            if(loyalSubjects == 1){
                if(targetPiece.type == Bishop && dirIdx >= 4){
                    // Bishops xray the diagonal
                    MarkPinnedMoves(game, possiblePin, kingSquare, targetSquare, dirIdx);
                } else if(targetPiece.type == Rook && dirIdx < 4){
                    // Rooks xray the vertical and horizontal
                    MarkPinnedMoves(game, possiblePin, kingSquare, targetSquare, dirIdx);
                } else if(targetPiece.type == Queen){
                    // Queens xrays from all dirs
                    MarkPinnedMoves(game, possiblePin, kingSquare, targetSquare, dirIdx);
                }
                break;
            }

            // Encounter enemy piece -> change dir regardless
            if(targetPiece.type != None) break;
        }
    }
}