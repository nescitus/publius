Publius is a dead simple bitboard chess engine for didactic purposes. Basically a boilerplate chess engine to build upon, no strings attached.

The current rating estimate is 2650 on the CCRL scale.

Functionalities up to date

- basic UCI support
- bitboard move generator
- pseudo-legal move list
- static exchange evaluator to detect bad captures
- perft
- alpha-beta search with PVS
- quiescence search
- transposition table
- draw detection
- null move
- late move reduction
- iterative deepening with aspiration windows
- piece/square tables
- mobility evaluation
- rooks on open files
- primitive king safety
- passed pawns
- isolated pawns
- very primitive evaluation of king's pawn shield
- very basic evaluation of non-winnable positions

Additional commands:
- in addition to "position startpos" there is "position kivipete" to test perft
- "step" command, accepting one or more moves and changing position on the board

Things to add if you want to contribute
1. bench command
2. a function to check whether a move is pseudolegal
4. with (2) in place staged move generator
5. comments
7. backward pawns
