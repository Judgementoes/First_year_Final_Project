// Custom defined library
#include "chess_board.h"
#include "chess_FEN.h"
#include "chess_moves.h"

// External graphics library
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const int screenWidth = 1440;
const int screenHeight = 900;


int main(){

    bool gameStart = false;

    // Variables which affects the board
    char *newdefaultPos = malloc(128*sizeof(char));
    char *FENPos = malloc(128*sizeof(char));
    strcpy(FENPos, defaultPos);
    strcpy(newdefaultPos, defaultPos);
    char *FEN_backup = malloc(128*sizeof(char));
    strcpy(FEN_backup, FENPos);
    int initial_W = 300;
    int initial_B = 300;
    int W_backup = initial_W;
    int B_backup = initial_B;
    bool FEN_editMode = false;
    bool Black_editMode = false;
    bool White_editMode = false;
    bool Increment_editMode = false;

    ChessClock clocks[2];
    int increment = 5;
    int i_backup = increment;

    // Game State init
    GameState game;

    // Analysis board init
    Piece chessboard[8][8];
    // Initialize chessboard pieces
    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            chessboard[i][j].type = None;
            chessboard[i][j].color = White;
            BlackAttacking[i*8+j] = false;
            WhiteAttacking[i*8+j] = false;
        }
    }

    // 2 arrays to keep track of the positions of the pieces of each color
    coord PieceLocations[32];
    for(int i = 0; i<32; i++){
        PieceLocations[i] = (coord){8, 8};
    }

    Pre_generateMoveData(NumSquarestoEdge);

    // Declare movelist
    Movelist moves;
    moves.capacity = 64;
    moves.count = 0;
    moves.move = NULL;

    // Load default chess position via FEN
    readFEN(FENPos, chessboard, PieceLocations, &game);

    // Turn-based variables
    enum PieceColor firstToMove = game.turn;

    // Variables for flipping the board
    enum PieceColor drawturn = game.turn;
    bool flipboard = true;

    // Update initial check and pin state before the main loop
    checkforChecks(chessboard, PieceLocations, &game);
    updatePinnedPieces(chessboard, &game);

    // Initial undo stack declaration
    UnmakeInfo *unmakeStack = NULL;

    GenerateMoveList(&moves, chessboard, PieceLocations, &game);

    // Init the clocks
    clocks[White].time = (double)initial_W;
    clocks[Black].time = (double)initial_B;
    clocks[White].ticktock = (game.turn == White);
    clocks[Black].ticktock = (game.turn == Black);
    clocks[White].pause = clocks[Black].pause = GetTime();

    InitWindow(screenWidth, screenHeight, "Chess in C");
    InitAudioDevice();

    // Load the main font of the game
    Font font = LoadFontEx("resource/Font/MazinDEMO-SemiBold.otf", 32, NULL, 0);

    // Initialize buttons
    Rectangle resignBtn = {1127, 750, 150, 50};
    Rectangle drawBtn = {1283, 750, 150, 50};
    Rectangle menuRect = {screenWidth/2 - 500, screenHeight/2 - 425, 1000, 850};
    Rectangle FENRect = {menuRect.x + 100, menuRect.y+140, 800, 40};
    Rectangle WhiteRect = {menuRect.x + 350, menuRect.y+290, 100, 40};
    Rectangle BlackRect = {menuRect.x + 350, menuRect.y+360, 100, 40};
    Rectangle IncrementRect = {menuRect.x + 350, menuRect.y+430, 100, 40};
    Rectangle StartBtn = {menuRect.x + 335, menuRect.y + 750, 150, 75};
    Rectangle ContinueBtn = {menuRect.x + 515, menuRect.y + 750, 150, 75};
    Rectangle AutoFlipBtn = {1126, 50, 50, 24};
    Rectangle FlipBtn = {1126 , 80, 50, 24};
    Rectangle ExportBtn = {menuRect.x + 300, menuRect.y + 100, 220, 30};

    // Load the textures for the pieces

    Texture2D pieceTextures[2][6]; // [PieceColor][PieceType] *see enum structs

    // Load texture and apply filter for black pieces
    pieceTextures[Black][Pawn] = LoadTexture("resource/texture/BlackPawn.png");
    SetTextureFilter(pieceTextures[Black][Pawn], TEXTURE_FILTER_BILINEAR);
    pieceTextures[Black][Bishop] = LoadTexture("resource/texture/BlackBishop.png");
    SetTextureFilter(pieceTextures[Black][Bishop], TEXTURE_FILTER_BILINEAR);
    pieceTextures[Black][Knight] = LoadTexture("resource/texture/BlackKnight.png");
    SetTextureFilter(pieceTextures[Black][Knight], TEXTURE_FILTER_BILINEAR);
    pieceTextures[Black][Rook] = LoadTexture("resource/texture/BlackRook.png");
    SetTextureFilter(pieceTextures[Black][Rook], TEXTURE_FILTER_BILINEAR);
    pieceTextures[Black][King] = LoadTexture("resource/texture/BlackKing.png");
    SetTextureFilter(pieceTextures[Black][King], TEXTURE_FILTER_BILINEAR);
    pieceTextures[Black][Queen] = LoadTexture("resource/texture/BlackQueen.png");
    SetTextureFilter(pieceTextures[Black][Queen], TEXTURE_FILTER_BILINEAR);
    
    // Load texture and apply filter for white pieces
    pieceTextures[White][Pawn] = LoadTexture("resource/texture/WhitePawn.png");
    SetTextureFilter(pieceTextures[White][Pawn], TEXTURE_FILTER_BILINEAR);
    pieceTextures[White][Bishop] = LoadTexture("resource/texture/WhiteBishop.png");
    SetTextureFilter(pieceTextures[White][Bishop], TEXTURE_FILTER_BILINEAR);
    pieceTextures[White][Knight] = LoadTexture("resource/texture/WhiteKnight.png");
    SetTextureFilter(pieceTextures[White][Knight], TEXTURE_FILTER_BILINEAR);
    pieceTextures[White][Rook] = LoadTexture("resource/texture/WhiteRook.png");
    SetTextureFilter(pieceTextures[White][Rook], TEXTURE_FILTER_BILINEAR);
    pieceTextures[White][King] = LoadTexture("resource/texture/WhiteKing.png");
    SetTextureFilter(pieceTextures[White][King], TEXTURE_FILTER_BILINEAR);
    pieceTextures[White][Queen] = LoadTexture("resource/texture/WhiteQueen.png");
    SetTextureFilter(pieceTextures[White][Queen], TEXTURE_FILTER_BILINEAR);

    // Load sound data for certain events
    Sound move_s = LoadSound("resource/audio/chess_move.wav");
    Sound capture_s = LoadSound("resource/audio/chess_capture.wav");
    Sound check_s = LoadSound("resource/audio/check.mp3");
    SetSoundVolume(check_s, 1.0f);
    Sound castle_s = LoadSound("resource/audio/castle.mp3");
    SetSoundVolume(castle_s, 1.0f);
    Sound promote_s = LoadSound("resource/audio/promote.mp3");
    SetSoundVolume(promote_s, 1.0f);
    Sound end_s = LoadSound("resource/audio/end.mp3");
    Sound start_s = LoadSound("resource/audio/start.mp3");
    

    SetTargetFPS(120);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        // Event flags
        static bool pieceselected = false;
        static bool newMove_isMade = false;
        static bool is_Promoting = false;
        static bool game_end = false;
        static bool insufficient = false;
        static bool resigned = false;
        static bool drawoffered = false;
        static bool retry = false;
        static bool dragging = false;
        // Moves management
        static char PawnPromote = '\0';
        static coord src = (coord){8, 8};
        static coord dst = (coord){8, 8};
        static coord dragdst = (coord){8, 8};
        static Move move;
        static coord possibledst[16];
        static int movecount = 0;
        // Moves animation
        static PieceAnimation animation;

        // Update
        //----------------------------------------------------------------------------------

        if(!gameStart){
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                Vector2 mouse = GetMousePosition();
                if(CheckCollisionPointRec(mouse, FENRect)) FEN_editMode = true;
                else FEN_editMode = false;
                if(CheckCollisionPointRec(mouse, WhiteRect)) White_editMode = true;
                else White_editMode = false;
                if(CheckCollisionPointRec(mouse, BlackRect)) Black_editMode = true;
                else Black_editMode = false;
                if(CheckCollisionPointRec(mouse, IncrementRect)) Increment_editMode = true;
                else Increment_editMode = false;
            }
            goto end;
        }

        Vector2 mouse_hover = GetMousePosition();
        bool resign_hovered = CheckCollisionPointRec(mouse_hover, resignBtn);
        bool draw_hovered = CheckCollisionPointRec(mouse_hover, drawBtn);
        bool auto_hovered = CheckCollisionPointRec(mouse_hover, AutoFlipBtn);
        bool flip_hovered = CheckCollisionPointRec(mouse_hover, FlipBtn);

        if(dragging && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            dragging = false;
            coord mousecoord = ScreentoBoard(mouse_hover, drawturn);
            if(checkBound(mousecoord)){ // Dropped the piece somewhere on the board
                dst = mousecoord;
                animation.is_drag = true; // disable the animation
                src = src;
                animation.src = animation.src;
                animation.dst = animation.dst;
                goto validate; // Validate the move
            }
        }

        if(!game_end && gameStart){
            double now = GetTime();
            for(int side = 0; side < 2; side++){
                if(clocks[side].ticktock){
                    clocks[side].time -= (now - clocks[side].pause);
                    if(clocks[side].time < 0.0) clocks[side].time = 0.0;
                }
                clocks[side].pause = now;
            }
        }

        if(clocks[White].time == 0.0 || clocks[Black].time == 0.0){
            PlaySound(end_s);
            game_end = true;
        }

        if(game_end) {
            if(IsKeyPressed(KEY_ENTER)){
                PlaySound(start_s);
                retry = true;
            }
            goto end;
        }

        if(is_Promoting) goto Promoting;

        // Detect mouse clicks
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Vector2 mouse = GetMousePosition();

            if(resign_hovered){
                if(!resigned) resigned = true;
                else if(resigned){
                    PlaySound(end_s);
                    game_end = true;
                    goto deselect;
                }
            } else if(draw_hovered){
                if(!drawoffered) drawoffered = true;
                else if(drawoffered){
                    PlaySound(end_s);
                    game_end = true;
                    goto deselect;
                }
            } else if(flip_hovered){
                if(drawturn == White) drawturn = Black;
                else drawturn = White;
            } else if(auto_hovered){
                flipboard = !flipboard;
            }

            if(!draw_hovered) drawoffered = false; // Clicking anywhere else besides the the draw button is an automatic rejection
            if(!resign_hovered) resigned = false; // Same logic for resign

            coord chesscoord = ScreentoBoard(mouse, drawturn);
            // Clicking outside the board deselects the piece BUT pieces can still be dragged out of the board
            if(!checkBound(chesscoord) && !dragging) goto deselect;

            Piece Selectedpiece = chessboard[chesscoord.y][chesscoord.x];

            if(!dragging && !pieceselected){
                select:
                chesscoord = ScreentoBoard(mouse, drawturn);
                Selectedpiece = chessboard[chesscoord.y][chesscoord.x];
                if(Selectedpiece.type == None || Selectedpiece.color != game.turn) goto deselect;
                PawnPromote = '\0';
                src = chesscoord;
                animation.src = src;
                //printf("src: %d %d\n", chesscoord.x, chesscoord.y);
                //printf("Animation at select:\nsrc: %d %d\ndst: %d %d\n", animation.src.x, animation.src.y, animation.dst.x, animation.dst.y);
                animation.src = animation.src;
                animation.dst = animation.dst;
                movecount = 0;
                for(int i = 0; i<moves.count; i++){
                    if(Coordscmp(moves.move[i].src, src)){
                        possibledst[movecount++] = moves.move[i].dst;
                        if(!Coordscmp(moves.move[i+1].src, src)) break; // exit early since the legal moves of a piece are stored next to each other
                    }
                }
                //
                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) dragging = true;
                pieceselected = true;
            } else if(!dragging && pieceselected){
                dst = chesscoord;
                Piece piece = chessboard[src.y][src.x];
                Piece Targetpiece = chessboard[dst.y][dst.x];
                //printf("dst: %d %d\n", chesscoord.x, chesscoord.y);
                
                //printf("Animation at selected:\nsrc: %d %d\ndst: %d %d\n", animation.src.x, animation.src.y, animation.dst.x, animation.dst.y);
                animation.src = animation.src;
                animation.dst = animation.dst;

                // So that switching between the selected pieces is easier
                if(Coordscmp(src, dst)) goto deselect;

                if(Targetpiece.type != None && Targetpiece.color == game.turn){
                    if(!Coordscmp(src, dst)){
                        goto select; // Handles the de-selecting logic
                    }
                }

                validate:
                piece = chessboard[src.y][src.x];
                Targetpiece = chessboard[dst.y][dst.x];
                move = (Move){src, dst};
                bool Movefound = false;
                // Check possible movelist
                for(int i = 0; i<moves.count; i++){
                    if(Movescmp(move, moves.move[i])) {
                        Movefound = true;
                        break;
                    }
                }
                if(Movefound){

                    // Promotion logic
                    if(piece.type == Pawn && !is_Promoting){
                        if((piece.color == White && dst.y == 7) || (piece.color == Black && dst.y == 0)){
                            is_Promoting = true;
                            goto end;
                        }
                    }
                    
                    // Some animation data
                    animation.active = true;
                    animation.src = src;
                    animation.dst = dst;
                    // printf("Animation:\nsrc: %d %d\ndst: %d %d\n", animation.src.x, animation.src.y, animation.dst.x, animation.dst.y);
                    animation.src = animation.src;
                    animation.dst = animation.dst;
                    animation.t = 0.0f;
                    animation.piece = piece;
                    if(Targetpiece.type != None) animation.Targetpiece = Targetpiece;
                    else animation.Targetpiece.type = None;
                    if(animation.piece.type == King && abs(animation.src.x - animation.dst.x) > 1){ // detect castling
                        animation.castle = true;
                        //Queen side
                        if(animation.dst.x == 2){
                            animation.castler.x = 3;
                            animation.castler.y = (game.turn == White)? 0:7;
                        } else if(animation.dst.x == 6){ // King side
                            animation.castler.x = 5;
                            animation.castler.y = (game.turn == White)? 0:7;
                        }
                    }
                    Promoting:
                    if(is_Promoting){
                        Vector2 Basepos = TranslateCoord_V2(dst, drawturn);
                        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                            Vector2 mouse = GetMousePosition();
                            for(int i = 0; i<4; i++){
                                Rectangle rect = {Basepos.x, Basepos.y + i * 100, 100, 100};
                                if(CheckCollisionPointRec(mouse, rect)){
                                    // Detected mouse click at location i
                                    switch(i){
                                        case 0: PawnPromote = 'q'; break;
                                        case 1: PawnPromote = 'n'; break;
                                        case 2: PawnPromote = 'r'; break;
                                        case 3: PawnPromote = 'b'; break;
                                        default: PawnPromote = '\0'; // If clicked out of bounds
                                    }
                                }
                            }
                            if(PawnPromote == '\0'){ // Player selected something else
                                is_Promoting = false;
                                pieceselected = false;
                                goto deselect;
                            }
                        }
                        if(PawnPromote == '\0') goto end; // Player hasn't selected anything
                    }

                    // Commit the move and update the board
                    clocks[game.turn].ticktock = false;
                    clocks[game.turn].time += (double)increment;
                    clocks[1-game.turn].ticktock = true;
                    clocks[1-game.turn].pause = GetTime();

                    // --- Make the move on the board ---
                    makeMove(chessboard, PieceLocations, move, &game, PawnPromote, &unmakeStack);
                    // --- Board has changed state after this ---
                    
                    
                    // Play some sounds
                    if(game.BlackinCheck || game.WhiteinCheck) PlaySound(check_s); // move result in check
                    else if(piece.type == King && abs(dst.x - src.x) > 1) PlaySound(castle_s); // move was a castle
                    else if(PawnPromote != '\0') PlaySound(promote_s);
                    else if(Targetpiece.type == None){
                        PlaySound(move_s); // regular move
                    } else if(Targetpiece.type != None){
                        PlaySound(capture_s); // a capture
                    }
                    exportFEN(chessboard, &game, FENPos);
                    is_Promoting = false;
                    PawnPromote = '\0';
                    newMove_isMade = true;
                    src = (coord){8, 8};
                    dst = (coord){8, 8};
                } else if(animation.is_drag){
                    animation.is_drag = false;
                    goto end; // skips deselection
                }
                deselect:
                pieceselected = false;
                src = (coord){8, 8};
                dst = (coord){8, 8};
                movecount = 0;
            }
        }

        if(insufficientmaterialcheck(chessboard, PieceLocations, game.turn)){
            insufficient = true;
            game_end = true;
            PlaySound(end_s);
        } else insufficient = false;

        if(game.HalfmoveClock == 50){
            PlaySound(end_s);
            game_end = true;
        }

        if(IsKeyPressed(KEY_U)){
            resigned = false;
            drawoffered = false;
            if(unmakeStack != NULL){

                // Reset event flags
                newMove_isMade = false;
                is_Promoting = false;
                resigned = false;
                insufficient = false;
                drawoffered = false;
                game_end = false;
                retry = false;
                PawnPromote = '\0';

                // Animation data for unmake
                Piece piece = unmakeStack->prevPiece;
                animation.active = true;
                animation.castle = false;
                animation.uncastle = false;
                /// The piece is moving backwards
                animation.src = unmakeStack->prevMove.dst;
                animation.dst = unmakeStack->prevMove.src;
                animation.src = animation.src;
                animation.dst = animation.dst;
                animation.t = 0.0f;
                animation.piece = piece;
                animation.Targetpiece.type = None; // The last square that the piece moved from is now a blank square
                if(animation.piece.type == King && abs(animation.src.x - animation.dst.x) > 1){ // detect castling
                    animation.uncastle = true;
                    //Queen side
                    if(animation.src.x == 2){
                        animation.castler.x = 0;
                        animation.castler.y = (animation.piece.color == White)? 0:7;
                    } else if(animation.src.x == 6){ // King side
                        animation.castler.x = 7;
                        animation.castler.y = (animation.piece.color == White)? 0:7;
                    }
                }
                // undo move
                Piece unpiece = unmakeStack->prevPiece;
                Piece unTargetpiece = unmakeStack->capturedPiece;
                Move unmove = unmakeStack->prevMove;
                bool unPromote = unmakeStack->Promoted;
                GameState ungame = unmakeStack->prevGame;
                
                // --- Un-play the previous move ---
                unmakeMove(&unmakeStack, &game, chessboard, PieceLocations);
                // --- Board state has been undone after this ---

                // Play some sounds
                if(ungame.BlackinCheck || ungame.WhiteinCheck) PlaySound(check_s); // move result in check
                else if(unpiece.type == King && abs(unmove.dst.x - unmove.src.x) > 1) PlaySound(castle_s); // move was a castle
                else if(unPromote) PlaySound(promote_s);
                else if(unTargetpiece.type == None){
                    PlaySound(move_s); // regular move
                } else if(unTargetpiece.type != None){
                    PlaySound(capture_s); // a capture
                }
                // re-generate old move list
                GenerateMoveList(&moves, chessboard, PieceLocations, &game);
            }
        }

        // Key bind for resign and draw offer
        if(IsKeyPressed(KEY_R)){
            if(!resigned){
                drawoffered = false;
                resigned = true;
            }else {
                PlaySound(end_s);
                game_end = true;
            }
        }

        if(IsKeyPressed(KEY_D)){
            if(!drawoffered){
                resigned = false;
                drawoffered = true;
            }else {
                PlaySound(end_s);
                game_end = true;
           }
        }

        if(IsKeyPressed(KEY_SPACE)){
            drawoffered = false;
            resigned = false;
            strcpy(FEN_backup, FENPos);
            B_backup = initial_B;
            W_backup = initial_W;
            i_backup = increment;
            gameStart = false;
        }


        if(newMove_isMade){
            GenerateMoveList(&moves, chessboard, PieceLocations, &game);
            if(moves.count == 0){
                PlaySound(end_s);
                game_end = true;
            }
            newMove_isMade = false;
        }

        end:
        if(retry){;

            // Reset event flags
            pieceselected = false;
            newMove_isMade = false;
            is_Promoting = false;
            insufficient = false;
            resigned = false;
            drawoffered = false;
            game_end = false;
            retry = false;

            src = (coord){8, 8};
            dst = (coord){8, 8};

            animation.active = false;
            animation.castle = false;
            animation.src = (coord){8, 8};
            animation.dst = (coord){8, 8};

            // Reset FEN pos
            strcpy(FENPos, newdefaultPos);

            // Reset board to the beginning state

            // Initialize chessboard pieces
            for(int i = 0; i<8; i++){
                for(int j = 0; j<8; j++){
                    chessboard[i][j].type = None;
                    chessboard[i][j].color = White;
                    BlackAttacking[i*8+j] = false;
                    WhiteAttacking[i*8+j] = false;
                }
            }

            moves.capacity = 64;
            moves.count = 0;
            moves.move = NULL;

            // Load default chess position via FEN
            readFEN(FENPos, chessboard, PieceLocations, &game);

            firstToMove = game.turn;

            // Update initial check and pin state before the main loop
            checkforChecks(chessboard, PieceLocations, &game);
            updatePinnedPieces(chessboard, &game);

            // Initial undo stack declaration
            unmakeStack = NULL;

            GenerateMoveList(&moves, chessboard, PieceLocations, &game);

            // Init the clocks
            clocks[White].time = (double)initial_W;
            clocks[Black].time = (double)initial_B;
            clocks[White].ticktock = (game.turn == White);
            clocks[Black].ticktock = (game.turn == Black);
            clocks[White].pause = clocks[Black].pause = GetTime();
        }
        

        //----------------------------------------------------------------------------------

        //Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(ChessBackground);

            DrawRectangleRounded(resignBtn, 0.2f, 10, (resign_hovered)? GRAY:DARKGRAY);
            DrawRectangleRounded(drawBtn, 0.2f, 10, (draw_hovered)? GRAY:DARKGRAY);
            if(flipboard){
                DrawRectangleRec(AutoFlipBtn, (auto_hovered)? GREEN:LIME);
            } else if(!flipboard){
                DrawRectangleRec(AutoFlipBtn, (auto_hovered)? RED:MAROON);
            }
            DrawRectangleRec(FlipBtn, (flip_hovered)? GRAY:DARKGRAY);

            if(!resigned) DrawText("Resign", resignBtn.x + 42, resignBtn.y + 16, 20, BLACK);
            else if(resigned) DrawText("Confirm", resignBtn.x + 38, resignBtn.y + 16, 20, BLACK);
            if(!drawoffered) DrawText("Offer draw", drawBtn.x + 19, drawBtn.y + 16, 20, BLACK);
            else if(drawoffered) DrawText("Accept draw", drawBtn.x + 12, drawBtn.y + 16, 20, BLACK);
            DrawText("Auto", AutoFlipBtn.x+2, AutoFlipBtn.y+2, 20, BLACK);
            if(drawturn == White) DrawText("Flip", FlipBtn.x+8, FlipBtn.y+2, 20, WHITE);
            else if(drawturn == Black) DrawText("Flip",FlipBtn.x+8, FlipBtn.y+1, 20, BLACK);

            drawBoard(drawturn, font);

            // Draw the captured pieces
            drawCapturedPieces(chessboard, PieceLocations, pieceTextures, drawturn);

            if(drawturn == Black){
                Rectangle rectblack = {1229, 468, 110, 60};
                Rectangle rectwhite = {1229, 368, 110, 60};
                DrawRectangleRounded(rectwhite, 0.2f, 10, WHITE); // White clock background
                DrawRectangleRounded(rectblack, 0.2f, 10, BLACK); // Black clock background
                DrawRectangleRounded(rectwhite, 0.2f, 10, Fade(GRAY, 0.45f)); // Grey out the clock that is not ticking
                DrawText(TextFormat("%d:%02d", (int)clocks[White].time/60, (int)clocks[White].time%60), 1247, 385, 30, BLACK);
                DrawText(TextFormat("%d:%02d", (int)clocks[Black].time/60, (int)clocks[Black].time%60), 1247, 485, 30, WHITE);
            } else {
                Rectangle rectwhite = {109, 468, 110, 60};
                Rectangle rectblack = {109, 368, 110, 60};
                DrawRectangleRounded(rectwhite, 0.2f, 10, WHITE); // White clock background
                DrawRectangleRounded(rectblack, 0.2f, 10, BLACK); // Black clock background
                DrawRectangleRounded(rectblack, 0.2f, 10, Fade(GRAY, 0.45f)); // Grey out the clock that is not ticking
                DrawText(TextFormat("%d:%02d", (int)clocks[Black].time/60, (int)clocks[Black].time%60), 127, 385, 30, WHITE);
                DrawText(TextFormat("%d:%02d", (int)clocks[White].time/60, (int)clocks[White].time%60), 127, 485, 30, BLACK);
            }

            // Highlight the previous move
            if(unmakeStack != NULL){
                Vector2 start = TranslateCoord_V2_decorate(unmakeStack->prevMove.src, drawturn);
                Vector2 end = TranslateCoord_V2_decorate(unmakeStack->prevMove.dst, drawturn);
                drawHighlightedSquare(start);
                drawHighlightedSquare(end);
            }

            // draw the possible moves
            if(pieceselected){
                Vector2 pos = TranslateCoord_V2_decorate(src, drawturn);
                drawHighlightedSquare(pos);
                for(int i = 0; i<movecount; i++){
                    pos = TranslateCoord_V2_decorate(possibledst[i], drawturn);
                    Piece Targetpiece = chessboard[possibledst[i].y][possibledst[i].x];
                    if(Targetpiece.type == None) DrawCircle(pos.x+50, pos.y+50, 17.5f, ChessGHOST);
                    else{
                        Color squareColor;
                        bool highlighted = false;
                        if(possibledst[i].y%2 == 0) squareColor = (possibledst[i].x%2 != 0)? ChessWHITE:ChessBROWN;
                        else squareColor = (possibledst[i].x%2 == 0)? ChessWHITE:ChessBROWN;
                        if(unmakeStack != NULL && Coordscmp(unmakeStack->prevMove.dst, possibledst[i])) highlighted = true;
                        drawCaptureCircle(pos, squareColor, highlighted);
                    }
                }
            }

            // Border indicates which square the mouse is hovering over
            if(dragging){
                coord drawcoord = ScreentoBoard(mouse_hover, drawturn);
                if(checkBound(drawcoord)){
                    Vector2 borderpos = TranslateCoord_V2_decorate(drawcoord, drawturn);
                    drawGrayBorder(borderpos);
                }
            }

            // animate the movement using linear interpolation
            if(animation.active){
                
                // Get the rook's location for castling or uncastling
                coord rookpos = (coord){8, 8};
                if(animation.castle){
                    if(animation.dst.x == 2) rookpos.x = 0;
                    else rookpos.x = 7;
                    rookpos.y = animation.dst.y;
                } else if(animation.uncastle){
                    if(animation.src.x == 2) rookpos.x = 3;
                    else rookpos.x = 5;
                    rookpos.y = animation.src.y;
                }

                // Interpolate board coordinates (as floats)
                float boardX = lerp((float)animation.src.x, (float)animation.dst.x, animation.t);
                float boardY = lerp((float)animation.src.y, (float)animation.dst.y, animation.t);

                // Convert to pixel coordinates
                Vector2 animatepos = TranslateCoord_V2_Float(boardX, boardY, drawturn);

                // Calculate speed
                float speed = Clamp(Vector2Distance((Vector2){animation.src.x, animation.src.y}, (Vector2){animation.dst.x, animation.dst.y}), 0.0f, 0.045f);

                // Draw the piece in motion
                if(!animation.is_drag) DrawTextureEx(pieceTextures[animation.piece.color][animation.piece.type], animatepos, 0.0f, 0.23f, WHITE);

                // Draw the rook castling
                if(animation.castle || animation.uncastle){
                    boardX = lerp((float)rookpos.x, (float)animation.castler.x, animation.t);
                    boardY = lerp((float)rookpos.y, (float)animation.castler.y, animation.t);
                    animatepos = TranslateCoord_V2_Float(boardX, boardY, drawturn);
                    DrawTextureEx(pieceTextures[animation.piece.color][Rook], animatepos, 0.0f, 0.23f, WHITE);
                }

                // Continue rendering the piece being captured;
                if(animation.t < 0.925f && animation.Targetpiece.type != None && !animation.is_drag){
                    DrawTextureEx(pieceTextures[animation.Targetpiece.color][animation.Targetpiece.type], TranslateCoord_V2(animation.dst, drawturn), 0.0f, 0.23f, WHITE);
                }

                // update the time
                animation.t += speed;
                
                // animation is finished
                if(animation.t >= 1.0f){
                    animation.t = 0.0f;
                    animation.active = false;
                    animation.is_drag = false;
                    animation.castle = false;
                    animation.uncastle = false;
                    animation.src = (coord){8, 8};
                    animation.dst = (coord){8, 8};
                    animation.castler = (coord){8, 8};
                }
            }

            // Draw highlight if the king is in check
            if(game.BlackinCheck){
                Vector2 pos = TranslateCoord_V2_decorate((coord){game.BlackKingIdx%8, game.BlackKingIdx/8}, drawturn);
                DrawCircleGradient(pos.x + 50, pos.y + 50, 60, MAROON, Fade(MAROON, 0.0f));
            } else if(game.WhiteinCheck){
                Vector2 pos = TranslateCoord_V2_decorate((coord){game.WhiteKingIdx%8, game.WhiteKingIdx/8}, drawturn);
                DrawCircleGradient(pos.x + 50, pos.y + 50, 60, MAROON, Fade(MAROON, 0.0f));
            }

            drawPieces(chessboard, PieceLocations, pieceTextures, drawturn, animation, dragging);

            if(dragging){
                Piece dragger = chessboard[src.y][src.x];
                DrawTextureEx(pieceTextures[dragger.color][dragger.type], (Vector2){mouse_hover.x - 50, mouse_hover.y - 50}, 0.0f, 0.23f, WHITE);
            }

            // The promotion choices overlays all other scenes
            if(is_Promoting){
                Piece piece = chessboard[src.y][src.x];
                Vector2 Basepos_piece = TranslateCoord_V2(dst, drawturn);
                Vector2 Basepos_board = TranslateCoord_V2_decorate(dst, drawturn);
                Rectangle shadowrect = {Basepos_board.x + 7, Basepos_board.y + 5, 100, 400};
                DrawRectangleGradientEx(shadowrect, BLACK, Fade(BLACK, 0.125f), Fade(BLACK, 0.1f), Fade(BLACK, 0.2f)); // Shadow effect;
                for(int i = 0; i<4; i++){
                    Rectangle rect = {Basepos_board.x, Basepos_board.y, 100, 100};
                    DrawRectangleRounded(rect, 0.2f, 10, WHITE);

                    if(i < 3){
                        DrawRectangle(Basepos_board.x, Basepos_board.y+75, 100, 75, WHITE);
                        
                    }
                    switch(i){
                        case 0: DrawTextureEx(pieceTextures[piece.color][Queen], Basepos_piece, 0.0f, 0.23f, WHITE); break;
                        case 1: DrawTextureEx(pieceTextures[piece.color][Knight], Basepos_piece, 0.0f, 0.23f, WHITE); break;
                        case 2: DrawTextureEx(pieceTextures[piece.color][Rook], Basepos_piece, 0.0f, 0.23f, WHITE); break;
                        case 3: DrawTextureEx(pieceTextures[piece.color][Bishop], Basepos_piece, 0.0f, 0.23f, WHITE); break;
                    }
                    Basepos_board.y+=100;
                    Basepos_piece.y+=100;
                }
                drawSegregation((Vector2){Basepos_board.x, Basepos_board.y-400+99});
            }

            if(game_end){
                if(drawoffered) DrawText("Draw!", 627, 385, 75, WHITE);
                else if(insufficient || game.HalfmoveClock == 50){
                    DrawText("Tie!", 657, 385, 75, WHITE);
                    if(insufficient) DrawText("(due to insufficient materials)", 492, 450, 30, WHITE);
                    if(game.HalfmoveClock == 50) DrawText("(due to fifty-move rule)", 539, 450, 30, WHITE);
                }
                else if(game.BlackinCheck){
                    DrawText("White wins!", 525, 385, 75, WHITE);
                    DrawText("By Checkmate!", 612, 450, 30, WHITE);
                } else if(clocks[Black].time == 0.0){
                    DrawText("White wins!", 525, 385, 75, WHITE);
                    DrawText(" By Timeout! ", 622, 450, 30, WHITE);
                } else if(game.turn == Black && resigned){
                    DrawText("White wins!", 525, 385, 75, WHITE);
                    DrawText("By Resignation", 607, 450, 30, WHITE);
                }
                else if(game.WhiteinCheck){
                    DrawText("Black wins!", 525, 385, 75, WHITE);
                    DrawText("By Checkmate!", 612, 450, 30, WHITE);
                } else if(clocks[White].time == 0.0){
                    DrawText("Black wins!", 525, 385, 75, WHITE);
                    DrawText(" By Timeout! ", 622, 450, 30, WHITE);
                } else if(game.turn == White && resigned){
                    DrawText("Black wins!", 525, 385, 75, WHITE);
                    DrawText("By Resignation", 607, 450, 30, WHITE);
                }
                else DrawText("Stalemate!", 527, 385, 75, WHITE);
                DrawText("Press Enter to restart", 412, 475, 50, WHITE);
            }

            if(!gameStart){
                // Dim the background
                Color Menu = (Color){79, 232, 255, 255};
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.25f));

                // Draw Menu
                DrawRectangleRounded(menuRect, 0.05f, 10, Fade(ChessMenu, 0.45f));

                DrawText("Game Setup", menuRect.x + 355, menuRect.y + 25, 50, Menu);
                
                // Some buttons and texts to edit FEN and time control
                DrawText("FEN string:", menuRect.x + 100, menuRect.y + 100, 30, Menu);
                int oldSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
                GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

                // Draw the input boxes
                GuiTextBox(FENRect, FENPos, 69, FEN_editMode);
                DrawText("Time control:", menuRect.x + 100, menuRect.y + 250, 30, Menu);
                GuiValueBox(WhiteRect, "White's time (s): ", &initial_W, 1, 86400, White_editMode);
                GuiValueBox(BlackRect, "Black's time (s): ", &initial_B, 1, 86400, Black_editMode);
                GuiValueBox(IncrementRect, "Increment (s): ", &increment, 1, 30, Increment_editMode);

                if(GuiButton((StartBtn), "Start")){
                    strcpy(newdefaultPos, FENPos);
                    gameStart = true;
                    game_end = true;
                    retry = true;
                    PlaySound(start_s);
                    goto end;
                }

                if(GuiButton((ContinueBtn), "Continue")){
                    FENPos = FEN_backup;
                    initial_W = W_backup;
                    initial_B = B_backup;
                    increment = i_backup;
                    gameStart = true;
                }

                if(GuiButton((ExportBtn), "Export FEN")){
                    printf("FEN string: %s\n", FENPos);
                }

                GuiSetStyle(DEFAULT, TEXT_SIZE, oldSize);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------

        if(flipboard) drawturn = game.turn;
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    for(int i = 0; i<6; i++){
        UnloadTexture(pieceTextures[Black][i]);
        UnloadTexture(pieceTextures[White][i]);
    }

    UnloadSound(move_s);
    UnloadSound(check_s);
    UnloadSound(capture_s);
    UnloadSound(end_s);
    UnloadSound(promote_s);
    UnloadSound(castle_s);
    UnloadSound(start_s);

    UnloadFont(font);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------x

    free(FENPos);
    free(moves.move);

    return 0;
}

// Old code for running chess on the terminal.

    // // Graphical board init
    // char board[BV][BH];
    // for(int i = 0; i<BV; i++){
    //     for(int j = 0; j<BH; j++){
    //         if(i < 17 && j < 33){
    //             if(i%2 == 0){
    //                 if(j%4 == 0) board[i][j] = '+';
    //                 else board[i][j] = '-';
    //             }
    //             else if(i%2 != 0){
    //                 if(j%4 == 0) board[i][j] = '|';
    //                 else board[i][j] = ' ';
    //             }
    //         } else board[i][j] = ' ';
    //     }
    // }

    // // Denote files
    // for(int i = 2; i<33; i+=4) board[17][i] = (char)('a'+(i-2)/4);
    // // Denote ranks
    // for(int i = 1; i<17; i+=2) board[i][34] = (char)('8'-(i-1)/2);

    // Main game loop
    // while(true)
    // {
    //     GenerateMoveList(&moves, chessboard, PieceLocations, &game);
    //     int promoteIdx = 0;
    //     printCheckStatus(&game);
    //     char input[8];
    //     char PawnPromote;
    //     fgets(input, sizeof(input), stdin);
    //     // Strip newline if present
    //     input[strcspn(input, "\n")] = 0;
    //     // Exit the game
    //     if(input[0] == '0') break;
    //     // Player wants to undo move
    //     if(strcmp(input, "undo") == 0){
    //         if(unmakeStack == NULL){
    //             printf("Cannot undo move - No moves have been made\n");
    //             continue;
    //         }
    //         unmakeMove(&unmakeStack, &game, chessboard, PieceLocations);
    //         updateBoard(board, chessboard, PieceLocations);
    //         printBoard(board, game.turn);
    //         continue;
    //     }
    //     Move move = NotationtoMove(input, &PawnPromote);
    //     // Move out of bounds
    //     if(!checkBound(move.src) || !checkBound(move.dst)){
    //         printf("Invalid move - Move out of the board\n");
    //         continue;
    //     }
    //     // Invalid pawn promotion
    //     if(PawnPromote != '\0')
    //     {
    //         if(chessboard[move.src.y][move.src.x].type != Pawn || (move.dst.y != 0 && move.dst.y != 7) || PawnPromote == '-'){
    //             printf("Invalid pawn promotion\n");
    //             continue;
    //         }
    //     }
    //     // If pawn moves to final rank but does not promote
    //     if(chessboard[move.src.y][move.src.x].type == Pawn){
    //         if(chessboard[move.src.y][move.src.x].color == White){
    //             if(move.dst.y == 7 && PawnPromote == '\0'){
    //                 printf("Invalid move - Must promote Pawn\n");
    //                 continue;
    //             }
    //         } else{
    //             if(move.dst.y == 0 && PawnPromote == '\0'){
    //                 printf("Invalid move - Must promote Pawn\n");
    //                 continue;
    //             }
    //         }
    //     }

    //     bool Movefound = false;
    //     // Check possible movelist
    //     for(int i = 0; i<moves.count; i++){
    //         if(Movescmp(move, moves.move[i])) {
    //             Movefound = true;
    //             break;
    //         }
    //     }
    //     if(Movefound){
    //         // commit to the move (makeMove will also update gamestate, checks, and pins)
    //         makeMove(chessboard, PieceLocations, move, &game, PawnPromote, &unmakeStack);
    //         // update board after move changes
    //         updateBoard(board, chessboard, PieceLocations);
    //     } else{
    //         printf("Invalid move\n");
    //         continue;
    //     }
    //     printBoard(board, game.turn);
    // }

// Debug: Print squares under attack by whites
    // for(int rank = 0; rank < 8; rank++){
    //     for(int file = 7; file >= 0; file--){
    //         if(WhiteAttacking[rank*8+file]) putchar('*');
    //         else putchar('.');
    //         putchar(' ');
    //     }
    //     putchar('\n');
    // }

// // Debug: print all generated moves
// printf("Number of possible moves: %d\n", moves.count);
// for(int i = 0; i < moves.count; i++) {
//     printf("Generated move: %c%d -> %c%d\n",
//         moves.move[i].src.x + 'a', moves.move[i].src.y + 1,
//         moves.move[i].dst.x + 'a', moves.move[i].dst.y + 1);
// }
// printf("Input move: %c%d -> %c%d\n",
//     move.src.x + 'a', move.src.y + 1,
//     move.dst.x + 'a', move.dst.y + 1);
        // Debug: print perft result for each moves at varying depths
        // for(int i = 0; i<moves.count; i++){
        //     Piece piece = chessboard[moves.move[i].src.y][moves.move[i].src.x];
        //     char PawnPromote = '\0';
        //     // Handle the four possible promotion paths for pawns
        //     if(piece.type == Pawn && ((piece.color == White && moves.move[i].dst.y == 7) || (piece.color == Black && moves.move[i].dst.y == 0))){
        //         switch(promoteIdx){
        //             case 0: PawnPromote = 'b'; promoteIdx++; break;
        //             case 1: PawnPromote = 'n'; promoteIdx++; break;
        //             case 2: PawnPromote = 'r'; promoteIdx++; break;
        //             case 3: PawnPromote = 'q'; promoteIdx = 0; break; // reset promoteIdx for future use
        //         }
        //     }
        //     makeMove(chessboard, PieceLocations, moves.move[i], &game, PawnPromote, &unmakeStack);
        //     printf("%c%d%c%d: %d\n", moves.move[i].src.x + 'a', moves.move[i].src.y + 1, moves.move[i].dst.x + 'a', moves.move[i].dst.y + 1, perftTest(chessboard, PieceLocations, &game,&unmakeStack, depth-1));
        //     unmakeMove(&unmakeStack, &game, chessboard, PieceLocations);
        // }

    // run a perft test for this position
    // int depth = 1;
    // stopwatch(&start);
    // printf("Nodes searched: %d\n", perftTest(chessboard, PieceLocations, &game, &unmakeStack, depth));
    // stopwatch(&start);