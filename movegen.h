struct Movelist
{
    int moves[256];
    int count;

    Movelist() {
        count = 0;
    }

    void add_move(int move);
    void reset();
};

int encode_move(
    int source, 
    int target, 
    int promotion,
    int capture,
    int double_pawn, 
    int enpassant, 
    int castling);

int decode_source(int move);

int decode_target(int move);

int decode_promotion(int move);

int decode_capture(int move);

int decode_double_pawn(int move);

int decode_enpassant(int move);

int decode_castling(int move);

int generate_pseudo_moves(Position &pos, Movelist &moves);

int generate_legal_moves(Position &pos, Movelist &moves);

void print_movelist(Movelist &moves);

