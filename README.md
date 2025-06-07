Publius is a dead simple public domain bitboard chess engine created for didactic purposes. Basically a boilerplate chess engine to build upon, no strings attached.

The current rating estimate is 2854 on the CCRL scale, based on the score against Fruit 2.1.

"Simple" and "teaching engine" are vague concepts, so explanation of constraints is in order:

- eval is done, if not necessarily tuned. It takes into account material, bishop pair, piece/square tables, mobility, passed, doubled and isolated pawns, intentionally primitive king safety and some detection of drawish endgames.
- quiescence search is probably done, unless I make delta pruning work.
- search is under active developement and there will be more stuff added.
- board update relies on four functions: AddPiece, DeletePiece, MovePiece and ChangePiece.
- apart from the hash key, there are no incremental updates of evaluation stuff. If someone wants to slap NNUE on top of my code, there will be no need to delete them. If not, at least eval can be read in one piece.
- movegen uses kindergarten bitboard technique. It's still slower than some libraries out there, but good enough.
- I'm still not sure whether to generate discovered checks.
- more comments will be added.

*Functionalities up to date*

GENERAL:

- basic UCI support
- kindergarten bitboard move generator
- pseudo-legal move list
- static exchange evaluator to detect bad captures
- perft
- bench

SEARCH:

- alpha-beta search with PVS
- iterative deepening with aspiration windows
- check-aware quiescence search with SEE pruning of bad captures
- transposition table
- draw detection
- null move (with verification)
- static null move / reverse futility pruning
- razoring
- late move reduction
- late move pruning
- futility pruning
- internal iterative reduction (slightly unusual form)
- singular extension

EVAL

- piece/square tables
- mobility evaluation
- rooks on open files and 7th rank
- primitive king safety
- passed pawns (plus control of the blockade square) 
- isolated pawns
- doubled pawns
- supported pawns (phalanx or defended)
- hanging pieces
- minor pieces attacking each other
- very primitive evaluation of king's pawn shield
- very basic evaluation of non-winnable positions

*Additional commands:*

- in addition to "position startpos" there is "position kivipete" to test perft
- "step" command, accepting one or more moves and changing position on the board
- "bench n", where n is the depth to which we search several positions
- "perft n", where n is the depth of perft test

*Things to add if you want to contribute*

1. a function to check whether a move is pseudolegal
2. with (1) in place staged move generator
3. comments, especially more on the kindergarten bitboards
4. backward pawns
