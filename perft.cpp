#include <iostream>
#include <chrono>
#include <cassert>
#include "position.h"
#include "movegen.h"

using namespace std;

#define copy_board(pos) \
    int board_copy[128], king_squares_copy[2], side_copy, enpassant_copy, castle_copy; \
    copy(pos.board, pos.board + 128, board_copy); \
    copy(pos.king_squares, pos.king_squares + 2, king_squares_copy); \
    side_copy = pos.side; \
    enpassant_copy = pos.enpassant; \
    castle_copy = pos.castle;

#define restore_board(pos) \
    copy(board_copy, board_copy + 128, pos.board); \
    copy(king_squares_copy, king_squares_copy + 2, pos.king_squares); \
    pos.side = side_copy; \
    pos.enpassant = enpassant_copy; \
    pos.castle = castle_copy;

// perft test
uint64_t perft(Position &pos, int depth, bool divide) {
    Movelist moves;
    int n_moves;
    uint64_t nodes = 0;
    if (depth == 0) return 1;

    n_moves = generate_pseudo_moves(pos, moves);

    //generate_moves(moves);

    // // define board state copies
    // int board_copy[128], king_squares_copy[2], side_copy, enpassant_copy, castle_copy;
    // // copy board state
    // copy(pos.board, pos.board + 128, board_copy);
    // copy(pos.king_squares, pos.king_squares + 2, king_squares_copy);
    // side_copy = pos.side;
    // enpassant_copy = pos.enpassant;
    // castle_copy = pos.castle;

    //copy_board(pos);

    // copy board state
    Position copy = pos;
    
    for (int i = 0; i < moves.count; i++) {
        //print_board();
        if (pos.make_move(moves.moves[i])) {
            // pos.print_board();
            // getchar();
            uint64_t result = perft(pos, depth - 1, false);
            nodes += result;
            if (divide) {
                cout << Position::square_to_coord[decode_source(moves.moves[i])];
                cout << Position::square_to_coord[decode_target(moves.moves[i])];
                cout << " " << result << endl;
            }
            

            // restore board state
            // copy(board_copy, board_copy + 128, pos.board);
            // copy(king_squares_copy, king_squares_copy + 2, pos.king_squares);
            // pos.side = side_copy;
            // pos.enpassant = enpassant_copy;
            // pos.castle = castle_copy;

            //restore_board(pos);

            // restore board state
            pos = copy;

            // pos.print_board();
            // getchar();
        }
    }

    return nodes;
}

void run_perft(Position &pos, int depth) {
    auto start = chrono::high_resolution_clock::now();
    uint64_t result = perft(pos, depth, true);
    auto stop = chrono::high_resolution_clock::now();
    
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    cout << endl;
    cout << "Nodes visited:    " << result << endl;
    cout << "Total time:       " << duration.count() << " ms" << endl;
    cout << fixed << "Nodes per second: " << int(1000.0 * result / duration.count()) << endl;
}

void run_perft_tests() {
    Position pos;
    uint64_t result;
    uint64_t total_nodes = 0;

    auto start = chrono::high_resolution_clock::now();

    // test 1
    cout << "Test 1: Start position" << endl;
    pos.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    result = perft(pos, 5, true);
    assert(result ==  4865609);
    total_nodes += result;


    // test 2
    cout << endl << "Test 2: Kiwipete" << endl;
    pos.parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    result = perft(pos, 4, true);
    assert(result ==  4085603);
    total_nodes += result;

    // test 3
    cout << endl << "Test 3: Position 3" << endl;
    pos.parse_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    result = perft(pos, 5, true);
    assert(result == 674624);
    total_nodes += result;

    // test 4
    cout << endl << "Test 4: Position 4" << endl;
    pos.parse_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    result = perft(pos, 4, true);
    assert(result == 422333);
    total_nodes += result;

    // test 5
    cout << endl << "Test 5: Position 5" << endl;
    pos.parse_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    result = perft(pos, 4, true);
    assert(result ==  2103487);
    total_nodes += result;

    // test 6
    cout << endl << "Test 6: Position 6" << endl;
    pos.parse_fen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    result = perft(pos, 4, true);
    assert(result ==  3894594);
    total_nodes += result;

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
   
    cout << endl << "Perft tests passed" << endl;
    cout << "Nodes visited:    " << total_nodes << endl;
    cout << "Total time:       " << duration.count() << " ms" << endl;
    cout << fixed << "Nodes per second: " << int(1000.0 * total_nodes / duration.count()) << endl;
}