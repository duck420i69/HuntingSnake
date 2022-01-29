# HuntingSnake

THINGS TO DO:

4.1 Process the case the head of snake touches the its body. (kind of solved)
In the guideline, there is no solution of the case that the head of snake touches its body.
So, we need to stop the game and ask if the player wants to continue or not?

4.6 Provide main menu (kind of solved)
When coming to boardgame, we print the menu game, for example, “New Game”, “Load
Game”, “Settings”, … So, this helps the players to easily choose actions they want.

4.3 Keeping the length unchanged. (solved)
In the guideline, when moving the next level (SPEED++), the length of snake will reset
(default is 6). We need to keep this length unchanged when moving to next level. Only
when moving to MAX_SPEED, it will be reset.

4.2 Save/load. (State.cpp)
In this guideline, we cannot save and load the game. We need two features. When the
players hit ‘L’, we show the line requesting the players provide the filename to save.
When the players hit ‘T’, we show the line requesting the players provide the file to load.


4.4 Process the gate (State.cpp)
In the guideline, when a snake eats 4 foods (just for illustration), the snake will be moved
to next level automatically. However, we need to provide the gate for my snake come
through. When my snake comes thought this gate completely, it will be moved to next
level.

4.5 Provide animation (Render.cpp)
When snake touches the “wall”, the its body, or final-gate, we create some lively
animation for these events.


Note: The body of snake consists of all student-IDs of the team. For example, the team
has two students with two IDs: 1312918 and 1312920, then the initial body of snake is:
“1312918”, when snake eats “one food”, the 1st digit of the 2nd student id will be added,
for example “13129181”. If the length of snake is “13129181312920”, the 1st digit of the
1st student id will be repeated, for example: “131291813129201” …
