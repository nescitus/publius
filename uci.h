// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

bool ParseCommand(std::istringstream& stream, Position* pos);
void OnUciCommand();
void OnPositionCommand(std::istringstream& stream, Position* pos);
void OnStepCommand(std::istringstream& stream, Position* pos);
void OnGoCommand(std::istringstream& stream, Position* pos);
void OnSetOptionCommand(std::istringstream& stream);
void OnBenchCommand(std::istringstream& stream, Position* pos);
void OnPerftCommand(std::istringstream& stream, Position* pos);
std::string ToLower(const std::string& str);
bool IsSameOrLowercase(const std::string& str1, const std::string& str2);
void OnStopCommand();
void TryLoadingNNUE(const char* path);

static constexpr auto startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
static constexpr auto kiwipeteFen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
