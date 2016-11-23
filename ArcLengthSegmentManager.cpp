#include "ArcLengthSegmentManager.h"
#include "SplineManager.h"

#include <glm.hpp>
#include <iostream>

#define PI 3.14159265359f
#define DURATION_MULTIPLIER 10000

using glm::vec3;

#define t1 0.3f
#define t2 0.8f

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
	this->arcLengthErrorThreshold = arcLengthErrorThreshold;
	this->maxParameterIntervalAllowed = maxParameterIntervalAllowed;
	// Get max and min point
	SplineManager* splineManager = SplineManager::Instance();
	maxPoint = splineManager->GetLastPoint();
	minPoint = splineManager->GetFirstPoint();

	pair<ParametricValue, ParametricValue> segment;
	// Loop through all the segments until they are empty
	while (!mySegmentList.empty()) {
		// Get the topmost segment
		segment = mySegmentList.top();
		// Remove it from the list
		mySegmentList.pop();

		ParametricValue ua = segment.first;
		ParametricValue ub = segment.second;

		// Compute midpoint
		ParametricValue um = (ua + ub) / 2.0f;

		// Get the real position values of parametric values
		vec3 uaRealPoint = splineManager->GetPointAtParametricValue(ua);
		vec3 ubRealPoint = splineManager->GetPointAtParametricValue(ub);
		vec3 umRealPoint = splineManager->GetPointAtParametricValue(um);

		// Calculate length of first half
		//float firstSegmentLength = static_cast<float>((umRealPoint - uaRealPoint).length());
		float firstSegmentLength = glm::distance(umRealPoint, uaRealPoint);
		// Calculate length of second half
		//float secondSegmentLength = static_cast<float>((ubRealPoint - umRealPoint).length());
		float secondSegmentLength = glm::distance(ubRealPoint, umRealPoint);
		// Calculate length of entire segment
		//float segmentLength = static_cast<float>((ubRealPoint - uaRealPoint).length());
		float segmentLength = glm::distance(ubRealPoint, uaRealPoint);
		// Calculate distance value
		float distance = firstSegmentLength + secondSegmentLength - segmentLength;
		// If d is higher than error threshold and parameter interval is higher than max interval
		if (distance > arcLengthErrorThreshold && abs(ub - ua) > maxParameterIntervalAllowed) {
			// insert first half and second half to the segment list
			InsertSegment(um, ub);
			InsertSegment(ua, um);
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

	// Normalize the s values
	ArcLengthEntryTable::iterator iter = myArcLengthTable.begin();
	float total = myArcLengthTable[1.0f];
	while (iter != myArcLengthTable.end()) {
		iter->second = iter->second / total;
		myArcLengthReverseTable.insert(std::make_pair(iter->second, iter->first));
		++iter;
	}
}

ParametricValue ArcLengthSegmentManager::FindParametricValue(ArcLength s)
{
	// returns either s or higher
	//upper_bound and lower_bound use binary search
	ArcLengthReverseTable::iterator iter = myArcLengthReverseTable.upper_bound(s);
	if (iter == myArcLengthReverseTable.end()) {
		--iter;
	}
	float nextS = iter->first;
	float nextU = iter->second;
	--iter;
	float prevS = iter->first;
	float prevU = iter->second;
	float l = (s - prevS) / (nextS - prevS);
	return (1 - l) * prevU + l * nextU;

	// NON-BINARY
	/*float maxS = myArcLengthTable[1.0f];
	s *= maxS;
	float sCurrent, sNext;
	int index = 0;
	ArcLengthEntryTable::iterator current = myArcLengthTable.begin();
	ArcLengthEntryTable::iterator next = current;
	++next;
	while (next != myArcLengthTable.end()) {
		sCurrent = current->second;
		sNext = next->second;
		if (s >= sCurrent && s < sNext) {
			float uCurrent = current->first;
			float uNext = next->first;
			float l = (s - sCurrent) / (sNext - sCurrent);
			return ((1.0f - l) * uCurrent + l * uNext);
		}
		++current;
		++next;
	}

	return current->first;*/

}

ArcLength ArcLengthSegmentManager::GetArcLengthUsingTime(float t)
{
	float v0 = 2.0f / (1.0f - t1 + t2);
	float s;
	if (t >= 0 && t <= t1) {
		s = (v0 / (2.0f * t1)) * t * t;
	}
	else if (t > t1 && t < t2) {
		s = v0 * (t - (t1 / 2.0f));
	}
	else {
		float firstPartLeft = (v0 * (t - t2)) / (2.0f * (1.0f - t2));
		float firstPartRight = (2.0f - t - t2);
		float secondPart = v0 * (t2 - (t1 / 2.0f));
		s = firstPartLeft * firstPartRight + secondPart;
	}

	return s;
}

float ArcLengthSegmentManager::GetVelocity(float t)
{
	float v0 = 2.0f / (1.0f - t1 + t2);
	if (t >= 0 && t <= t1) {
		return (v0 * t) / t1;
	}
	else if (t > t1 && t < t2) {
		return v0;
	}
	else {
		return (v0 * (1 - t)) / (1 - t2);
	}
}

void ArcLengthSegmentManager::PrintArcLengthTable()
{
	ArcLengthEntryTable::iterator cIter = myArcLengthTable.begin();
	while (cIter != myArcLengthTable.end()) {
		std::cout << "Parametric Value u: " << cIter->first << " | ArcLength S: " << cIter->second << std::endl;
		++cIter;
	}
}
