# jacmat-chessboard

Electronic chessboard project where the goal is to be able to play aganist a computer, tablet or smartphone 
just moving the pieces physically on a board.
The idea is that the movements made on the board are sent via serial (USB) to the chosen peripheral, 
it receives them and transforms them into a play in the program against which it is played. 
The program will respond and the user will move the piece that the program has moved.

Therefore you need an external program, an USB connection and to move the opponent's pieces. 
It is also intended to be able to play two players and that the movements are transferred to the program 
so that the game can be saved in PGN on the peripheral used.

This code runs the JacMat usb electronic chessboard. The complete code for all the features is named FullCode,
in addition to this code I uploaded other small parts of code as the project progressed to make it more easy
to understand. 
The chessboard can be run whitout all the features, for example, to not play with time control you don't need 
the displays, the tft screen is also not prescindible so I decided to rewrite the code to simply play against
the computer, removing the displays, tft and arduino Nano card. This way you can built a more simply board.
I'll upload the rewriten code later.


https://jacmatchessboard.blogspot.com
