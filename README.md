# cities-game
Console client-server application – “Cities” word game.
This application implement game "Cities" (*).

To build: run make -f makefile

To begin playing should first start the server: to do this, run the application and press 1

To start the game, run the client:
To do this, run the application and press any key other than 1
Then select a difficulty level:
  0 - easy (you will be permitted 12 wrong answers, the opponent knows relatively little towns, your answer is not checked);
  1 - average (you will be allowed 6 wrong answers, the opponent knows relatively many cities, your answer is checked by cities opponent knows)
  2 - hard (you will be allowed 3 incorrect answers, the opponent knows the names of many cities, your option is checked by cities opponent knows)

The game ends in cases where:
  -used to limit the allowed incorrect answers (lose);
  -user surrendered (you should press the "-") (lose);
  -the opponent does not know the city (winn);

(*) Computer uses the names of Ukrainian cities only


Language/libraries: C/C++, STL;
https://github.com/pavloshiba/cities-game.git

