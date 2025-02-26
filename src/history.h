#pragma once

class HistoryData {
private:
	int killer1[PlyLimit];
	int killer2[PlyLimit];
	int history[12][64];
	void Trim(void);
	int HistoryData::Inc(const int depth);
public:
	void Clear(void);
	void Update(Position* pos, int mv, int d, int ply);
	int Get(Position* pos, int mv);
	int GetKiller1(const int ply);
	int GetKiller2(const int ply);
};

extern HistoryData History;