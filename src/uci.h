void UciLoop(void);
bool ParseCommand(std::istringstream& stream, Position* pos);
void OnUciCommand();
void OnPositionCommand(std::istringstream& stream, Position* pos);
void OnGoCommand(std::istringstream& stream, Position* pos);
void OnSetOptionCommand(std::istringstream& stream);
void OnPerftCommand(std::istringstream& stream, Position* p);
std::string ToLower(const std::string& str);
bool IsSameOrLowercase(const std::string& str1, const std::string& str2);
