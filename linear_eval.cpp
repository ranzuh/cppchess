#include <iostream>
#include <numeric>
#include <assert.h>
#include "position.h"
#include "linear_evaluation.h"
#include "perft.h"

//using namespace std;

int main() {
    Position game_position;
    game_position.parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    int features[380] = {0};
    extract_features(game_position, features);
    std::cout << "pawns " << features[0] << std::endl;
    std::cout << "knights " << features[1] << std::endl;
    std::cout << "bishops " << features[2] << std::endl;
    std::cout << "rooks " << features[3] << std::endl;
    std::cout << "queens " << features[4] << std::endl;

    int score = evaluate_features(game_position, features);
    
    std::cout << "eval " << score << std::endl;

    //run_perft_tests();
    //perft(game_position, 3, 1);

    for (size_t i = 0; i < 380; i++)
    {
        std::cout << features[i] << " ";
        if (i == 4) std::cout << std::endl;
        if (i > 4 && (i+4) % 8 == 0) std::cout << std::endl;
    }
    
}
