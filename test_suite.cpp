#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "assert.h"
#include "position.h"
#include "hashtable.h"
#include "uci.h"
#include "perft.h"
#include "movegen.h"

using namespace std;

/* 
movetime 5000 with optimizations 278/300 0.926667
movetime  500 with optimizations 251/300 0.836667
Note: Few tests fail because get_san_string() is not correct yet
eg. it fails to output Rab8, instead it outputs Rb8
*/
void test_wac_tactics(Position &pos, int movetime) {
    int correct = 0;
    int total = 0;
    double ratio = 0.0;

    std::ifstream infile("wac.epd");
    string line;
    while (std::getline(infile, line))
    {
        total++;
        pos.parse_fen(line);
        int bm = parse_go(pos, "go movetime " + std::to_string(movetime), false);
        string san = get_san_string(pos, bm);
        if (line.find(san) != string::npos == 1) correct++;
        else cout << "MISTAKE: " << line << " " << san << endl;;
        cout << "correct of total: " << correct << "/" << total << " ";
        ratio = correct / (double)total;
        cout << ratio << endl;
        // clear_hash_table();
    }

    assert(ratio > 0.8 && "wac tactics test failed");
    cout << "WAC tactics test passed" << endl;
}

int main() {
    init_random_keys();
    Position pos;
    clear_hash_table();

    test_wac_tactics(pos, 500);
    run_perft_tests();
}