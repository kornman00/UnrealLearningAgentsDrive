// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningNamesBPFL.h"

FName UEpicLearningNamesBPFL::LearningAgentsManagerName()
{
	static FName LiteralName{ "LearningAgentsManager" };
	return LiteralName;
}

FName UEpicLearningNamesBPFL::CarAgentActionNameSteering()
{
	static FName LiteralName{ "Steering" };
	return LiteralName;
}

FName UEpicLearningNamesBPFL::CarAgentActionNameThrottleBrake()
{
	static FName LiteralName{ "ThrottleBrake" };
	return LiteralName;
}

// Was: GetAgentRewardNameLocationDifferenceAboveThreshold
FName UEpicLearningNamesBPFL::CarAgentRewardNameLocationDifferenceAboveThreshold()
{
	static FName LiteralName{ "LocationDifferenceAboveThresholdReward" };
	return LiteralName;
}

// Was: GetAgentRewardNameVelocityAlongSpline
FName UEpicLearningNamesBPFL::CarAgentRewardNameVelocityAlongSpline()
{
	static FName LiteralName{ "VelocityAlongSplineReward" };
	return LiteralName;
}

// Was: GetAgentTrackObservationNameLocation
FName UEpicLearningNamesBPFL::TrackLookaheadObservationNameLocation()
{
	static FName LiteralName{ "Location" };
	return LiteralName;
}

// Was: GetAgentTrackObservationNameDirection
FName UEpicLearningNamesBPFL::TrackLookaheadObservationNameDirection()
{
	static FName LiteralName{ "Direction" };
	return LiteralName;
}

FName UEpicLearningNamesBPFL::CarObservationNameVelocity()
{
	static FName LiteralName{ "Velocity" };
	return LiteralName;
}

// Was: GetAgentObservationNameTrackLookahead
FName UEpicLearningNamesBPFL::CarAgentObservationNameTrackLookahead()
{
	static FName LiteralName{ "Track" };
	return LiteralName;
}

// Was: GetAgentObservationNameCarVelocity
FName UEpicLearningNamesBPFL::CarAgentObservationNameSelf()
{
	static FName LiteralName{ "Car" };
	return LiteralName;
}

FName UEpicLearningNamesBPFL::DefaultNameForLocationAlongSplineObservation()
{
	static FName LiteralName{ "LocationAlongSplineObservation" };
	return LiteralName;
}
FName UEpicLearningNamesBPFL::DefaultNameForDirectionAlongSplineObservation()
{
	static FName LiteralName{ "DirectionAlongSplineObservation" };
	return LiteralName;
}
FName UEpicLearningNamesBPFL::DefaultNameForVelocityObservation()
{
	static FName LiteralName{ "VelocityObservation" };
	return LiteralName;
}
