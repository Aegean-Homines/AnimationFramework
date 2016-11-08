#pragma once

#include <map>
#include <stack>

using std::map;
using std::stack;
using std::pair;

typedef float ArcLength; //s = G(u)
typedef float ParametricValue; //u

typedef map<ParametricValue, ArcLength> ArcLengthEntryTable;
typedef map<ArcLength, ParametricValue> ArcLengthReverseTable;
typedef stack<pair<ParametricValue, ParametricValue>> SegmentList;

class ArcLengthSegmentManager
{
public:
	~ArcLengthSegmentManager();
	// Singleton things
	static ArcLengthSegmentManager* Instance();

	// Segmentation Procedure
	void FillSegmentationTable(float arcLengthErrorThreshold, float maxParameterIntervalAllowed);
	ParametricValue FindParametricValue(ArcLength s);

	// v to s
	ArcLength GetArcLengthUsingTime(float t);

	// Calculate velocity at any time t
	float GetVelocity(float t);
	// Debugging
	void PrintArcLengthTable();
private:
	// Containers
	ArcLengthEntryTable myArcLengthTable;
	ArcLengthReverseTable myArcLengthReverseTable; // the entire reason for the map's existence is the easy binary search
	SegmentList mySegmentList;

	// Record of real data
	float maxPoint, minPoint;
	float arcLengthErrorThreshold, maxParameterIntervalAllowed;
	// Helpers
	void InitializeContainers();
	void InsertSegment(ParametricValue u1, ParametricValue u2);
	void InsertArcLength(ParametricValue u, ArcLength s);

	// #JustSingletonThings
	ArcLengthSegmentManager();
	ArcLengthSegmentManager(ArcLengthSegmentManager const &) {};
	ArcLengthSegmentManager& operator=(ArcLengthSegmentManager const &) {};
	static ArcLengthSegmentManager* instance;
};

