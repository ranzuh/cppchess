enum attacker_pieces : int { ap = 0x1, aP = 0x2, aN = 0x4, aB = 0x8, aR = 0x10, aQ = 0x20, aK = 0x40 };
int piece_to_attacker[] = { 0, ap, aN, aB, aR, aQ, aK, aP, aN, aB, aR, aQ, aK };
int attack_table[240] {
	//  0,    0,    0,    0,    0,    0,    0,    0,
	 0x28,    0,    0,    0,    0,    0,    0, 0x30,
	    0,    0,    0,    0,    0,    0, 0x28,    0,
		0, 0x28,    0,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0,    0, 0x28,    0,    0,
		0,    0, 0x28,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0, 0x28,    0,    0,    0,
		0,    0,    0, 0x28,    0,    0,    0, 0x30,
		0,    0,    0, 0x28,    0,    0,    0,    0,
		0,    0,    0,    0, 0x28,    0,    0, 0x30,
		0,    0, 0x28,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0, 0x28,  0x4, 0x30,
	  0x4, 0x28,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,  0x4, 0x69, 0x70,
	 0x69,  0x4,    0,    0,    0,    0,    0,    0,
	 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x70,    0,
	 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,    0,
		0,    0,    0,    0,    0,  0x4, 0x6A, 0x70,
	 0x6A,  0x4,    0,    0,    0,    0,    0,    0,   
		0,    0,    0,    0,    0, 0x28,  0x4, 0x30,
	  0x4, 0x28,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0, 0x28,    0,    0, 0x30,
		0,    0, 0x28,    0,    0,    0,    0,    0,
		0,    0,    0, 0x28,    0,    0,    0, 0x30,
		0,    0,    0, 0x28,    0,    0,    0,    0,
		0,    0, 0x28,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0, 0x28,    0,    0,    0,   
		0, 0x28,    0,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0,    0, 0x28,    0,    0,
	 0x28,    0,    0,    0,    0,    0,    0, 0x30,
		0,    0,    0,    0,    0,    0, 0x28,    0,
};

// calculate difference of squares for attack table
inline int square_diff(int attacker_sq, int defender_sq) {
    return attacker_sq - defender_sq + 0x77;
}

// is piece-type attacking (potentially) defender square from attacker square
inline int square_attacked_by(int piece, int attacker_sq, int defender_sq) {
	return (piece_to_attacker[piece] & attack_table[square_diff(attacker_sq, defender_sq)]) != 0;
}