This is the final project of the ECE2024 module "Introduction to Programming" taught by Professor Phong at VGU. Its aim is to be a chess engine with a playable GUI entirely in the C language.

The engine is written entirely in C and is the back-end on which the game operates
Raylib - an external graphics rendering library written in C - is the library of choice to handle the GUI for the front-end for a user-friendly experience.

Run the program by pressing the "Chess_in_C.exe" file.

Controls:

---Game Setup menu---
- The menu comprises of 1 text box and 3 value boxes for the user to input their desired game format and three buttons.
- The text box display the current position of the chess board in FEN, a notation system commonly used in chess programming.
  - User can input their own FEN to update the board.
  - Press "Export FEN" to have a copy/paste - ready FEN of the current position printed on the console.
- The 3 value boxes are the time control options.
  - User can input their desired time on the clock for black, white and how many second will be awarded to both after each move.
- The "Start" button will begin the game with the inputted FEN string and time control options. Press this if you want to start a new game with custom settings.
- The "Continue" button exits the menu. Press this if you want to resume the game without any changes.

---Chess game---
- The pieces can be moved via the cursor.
  - The program supports both point-and-click and drag-and-drop playstyle.
  - The program will automatically highlights the possible moves and captures of the selected pieces. It comforms very rigorously to the official rules of chess.
- There are 4 clickable buttons.
  - The "Auto" button, when active, automatically flips the orientation of the board and the pieces to the view point of the color which is to move. Disable this to have the board stay still on one side.
    - The "Flip" button can be use when "Auto" is disabled. It manually flips the orientation to each black's and white's perspective.
  - The "Resign" and "Offer draw" buttons are main mechanics of chess. Pressing twice on each button will end the game by Resignation or Draw immediately.
- Keybinds:
  - Players can also resign or offer draw by pressing the 'R' or 'D' keys on their keyboards respectively.
  - Pressing 'U' after a move is made will undo that move. Pressing repeatedly will restore the original position of the board.
  - Press the spacebar to bring up the menu at any time.
  - When the game is over, press 'Enter' to restart the game from the original position with the original time control.
 
Compilation method:

- The game is compiled using CMake, and comes pre-compiled.
- If you want to compile it yourself follow these steps:
  - Have CMake installed.
  - Open the terminal or file explore and navigate to the build folder. (YourFileStructure/First_year_Final_Project/build)
  - Run the line "cmake ..". This will tell CMake to link all of the files and folders.
  - Run the line "cmake --build ." to build the program, or "cmake --build . --clean-first" to have a clean build.
  - Run the program.
 
Thank you for showing interest for the project.
