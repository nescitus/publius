Publius is a dead simple bitboard chess engine for didactic purposes. Basically a boilerplate chess engine to build upon, no strings attached.

The current rating estimate is 2720 on the CCRL scale, based on narrowly beating Fruit 2.1.

Functionalities up to date

- basic UCI support
- bitboard move generator
- pseudo-legal move list
- static exchange evaluator to detect bad captures
- perft
- alpha-beta search with PVS
- check-aware quiescence search with SEE pruning of bad captures
- transposition table
- draw detection
- null move (with verification)
- late move reduction
- iterative deepening with aspiration windows
- piece/square tables
- mobility evaluation
- rooks on open files and 7th rank
- primitive king safety
- passed pawns
- isolated pawns
- doubled pawns
- very primitive evaluation of king's pawn shield
- very basic evaluation of non-winnable positions

Additional commands:
- in addition to "position startpos" there is "position kivipete" to test perft
- "step" command, accepting one or more moves and changing position on the board
- "bench n", where n is the depth to which we search several positions
- "perft n", where n is the depth of perft test

Things to add if you want to contribute

1. a function to check whether a move is pseudolegal
2. with (1) in place staged move generator
3. comments
4. backward pawns
5. faster bitboard move generator (kindergarten or magic)
