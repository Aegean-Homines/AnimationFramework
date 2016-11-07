#pragma once

#include <map>
#include <queue>

using std::map;
using std::queue;
using std::pair;

typedef float ArcLength; //s = G(u)
typedef float ParametricValue; //u

typedef map<ParametricValue, ArcLength> ArcLengthEntryTable;
typedef queue<pair<ParametricValue, ParametricValue>> SegmentList;

class ArcLengthSegmentManager
{
public:
	~ArcLengthSegmentManager();
	// Singleton things
	static ArcLengthSegmentManager* Instance();

	// Segmentation Procedure
	void FillSegmentationTable(float arcLengthErrorThreshold, float maxParameterIntervalAllowed);
private:
	// Containers
	ArcLengthEntryTable myArcLengthTable;
	SegmentList mySegmentList;

	// Record of real data
	float maxPoint, minPoint;

	// Helpers
	void InitializeContainers();
	void InsertSegment(ParametricValue u1, ParametricValue u2);
	void InsertArcLength(ParametricValue u, ArcLength s);
	float ParametricValueToRealValue(ParametricValue u);

	// #JustSingletonThings
	ArcLengthSegmentManager();
	ArcLengthSegmentManager(ArcLengthSegmentManager const &) {};
	ArcLengthSegmentManager& operator=(ArcLengthSegmentManager const &) {};
	static ArcLengthSegmentManager* instance;
};

