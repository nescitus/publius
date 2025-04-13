#pragma once

class HistoryData {
private:
	int killer1[PlyLimit];
	int killer2[PlyLimit];
	int cutoffHistory[12][64]; // [12][64][64] failed narrowly 2025-03-16
	int triesHistory[12][64];
	void Trim(void);
	int Inc(const int depth);
public:
	HistoryData(); // constructor
	void Clear(void);
	void Update(Position* pos, const int move, const int depth, const int ply);
	void UpdateTries(Position* pos, const int move, const int depth);
	int GetScore(Position* pos, const int move);
	int GetKiller1(const int ply);
	int GetKiller2(const int ply);
	bool IsKiller(const int move, const int ply);
};

extern HistoryData History;