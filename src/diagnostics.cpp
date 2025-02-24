#include "publius.h"

void PrintBoard(Position* pos) {

    char* piece_name[] = { "P ", "p ", "N ", "n ", "B ", "b ", "R ", "r ", "Q ", "q ", "K ", "k ", ". " };

    std::cout << "--------------------------------------------" << std::endl << "  ";

    for (Square sq = A1; sq < 64; ++sq) {

        Square mapSq = sq ^ 56;

        if (pos->IsEmpty(mapSq))
            std::cout << piece_name[pos->GetPiece(mapSq)];
        else
            std::cout << piece_name[pos->GetPiece(mapSq)];

        if ((sq + 1) % 8 == 0) {
            std::cout << "  " << 9 - ((sq + 1) / 8) << std::endl << "  ";
        }
    }

    std::cout << std::endl << "  a b c d e f g h" << std::endl << std::endl
        << "--------------------------------------------"
        << std::endl;
}

int Perft(Position* pos, int ply, int depth, bool isNoisy) {

    int move = 0;
    MoveList list;
    int localCount = 0;
    int moveCount = 0;

    list.Clear();
    FillCompleteList(pos, &list);
    list.ScoreMoves(pos, ply, 0);
    int moveListLength = list.GetInd();

    if (moveListLength) {
        for (int i = 0; i < moveListLength; i++) {

            move = list.GetMove();
            pos->DoMove(move, ply);

            if (pos->LeavesKingInCheck()) {
                pos->UndoMove(move, ply);
                continue;
            }

            if (depth == 1) {
                moveCount++;
                localCount = 1;
            }
            else {
                localCount = Perft(pos, ply + 1, depth - 1, isNoisy);
                moveCount += localCount;
            }

            pos->UndoMove(move, ply);

            if (ply == 0 && isNoisy)
                std::cout << MoveToString(move) << ": " << localCount << std::endl;
        }
    }

    return moveCount;
}
