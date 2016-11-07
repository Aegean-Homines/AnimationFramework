#include "ArcLengthSegmentManager.h"
#include "SplineManager.h"

ArcLengthSegmentManager* ArcLengthSegmentManager::instance = nullptr;


void ArcLengthSegmentManager::InitializeContainers()
{
	mySegmentList = SegmentList();
	myArcLengthTable.clear();

	// push the entire curve
	mySegmentList.push(std::make_pair(0.0f, 1.0f));
	// Initial entry at ua
	myArcLengthTable.insert(std::make_pair(0.0f, 0.0f));
}

void ArcLengthSegmentManager::InsertSegment(ParametricValue u1, ParametricValue u2)
{
	mySegmentList.push(std::make_pair(u1, u2));
}

void ArcLengthSegmentManager::InsertArcLength(ParametricValue u, ArcLength s)
{
	myArcLengthTable.insert(std::make_pair(u, s));
}

float ArcLengthSegmentManager::ParametricValueToRealValue(ParametricValue u)
{
	float realPointInterval = maxPoint - minPoint;
	return (u * realPointInterval) + minPoint;
}

ArcLengthSegmentManager::ArcLengthSegmentManager()
{
	InitializeContainers();
}

ArcLengthSegmentManager::~ArcLengthSegmentManager()
{

}

ArcLengthSegmentManager* ArcLengthSegmentManager::Instance()
{
	if (instance == nullptr)
		instance = new ArcLengthSegmentManager();

	return instance;
}

void ArcLengthSegmentManager::FillSegmentationTable(float arcLengthErrorThreshold, float maxParameterIntervalAllowed)
{
	// Clear everything
	InitializeContainers();

	// Get max and min point
	SplineManager* splineManager = SplineManager::Instance();
	maxPoint = splineManager->GetLastPoint();
	minPoint = splineManager->GetFirstPoint();

	pair<ParametricValue, ParametricValue> segment;
	// Loop through all the segments until they are empty
	while (!mySegmentList.empty()) {
		// Get the topmost segment
		segment = mySegmentList.front();
		// Remove it from the list
		mySegmentList.pop();

		ParametricValue ua = segment.first;
		ParametricValue ub = segment.second;

		// Compute midpoint
		ParametricValue um = (ua + ub) / 2.0f;

		// Get the real position values of parametric values
		float uaRealPoint = ParametricValueToRealValue(ua);
		float ubRealPoint = ParametricValueToRealValue(ub);
		float umRealPoint = ParametricValueToRealValue(um);

		// Calculate length of first half
		float firstSegmentLength = abs(umRealPoint - uaRealPoint);
		// Calculate length of second half
		float secondSegmentLength = abs(ubRealPoint - umRealPoint);
		// Calculate length of entire segment
		float segmentLength = abs(ubRealPoint - uaRealPoint);
		// Calculate distance value
		float distance = firstSegmentLength + secondSegmentLength - segmentLength;
		// If d is higher than error threshold and parameter interval is higher than max interval
		if (distance > arcLengthErrorThreshold && abs(ub - ua) > maxParameterIntervalAllowed) {
			// insert first half and second half to the segment list
			InsertSegment(ua, um);
			InsertSegment(um, ub);
		}
		else {
			// Calculate arc lengths
			ArcLength sm = myArcLengthTable[ua] + firstSegmentLength;
			ArcLength sb = sm + secondSegmentLength;
			// Record um and ub to the table
			InsertArcLength(um, sm);
			InsertArcLength(ub, sb);
		}
	}
}
