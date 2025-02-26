#pragma once

class HistoryData {
private:
	int killer[PlyLimit];
	int history[12][64];
	void Trim(void);
	int HistoryData::Inc(const int depth);
public:
	void Clear(void);
	void Update(Position* pos, int mv, int d, int ply);
	int Get(Position* pos, int mv);
	int GetKiller(int ply);
};

extern HistoryData History;