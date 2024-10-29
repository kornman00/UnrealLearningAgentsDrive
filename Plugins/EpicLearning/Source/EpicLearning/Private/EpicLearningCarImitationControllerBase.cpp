// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarImitationControllerBase.h"

#include "EpicLearningNamesBPFL.h"

void UEpicLearningCarImitationControllerBase::EvaluateAgentController_Implementation(
	FLearningAgentsActionObjectElement& OutActionObjectElement,
	ULearningAgentsActionObject* InActionObject,
	const ULearningAgentsObservationObject* InObservationObject,
	const FLearningAgentsObservationObjectElement& InObservationObjectElement,
	const int32 AgentId)
{
	float SteeringValue;
	float ThrottleBrakeValue;
	EvaluateAgentSteeringAndThrottleBrake(AgentId, SteeringValue, ThrottleBrakeValue);

	OutActionObjectElement =
		ULearningAgentsActions::MakeStructActionFromArrayViews(InActionObject,
			{
				UEpicLearningNamesBPFL::CarStateNameSteering(),
				UEpicLearningNamesBPFL::CarStateNameThrottleBrake(),
			},
			{
				ULearningAgentsActions::MakeFloatAction(InActionObject,
					SteeringValue, SteeringFloatActionScale, UEpicLearningNamesBPFL::CarStateNameSteering()),
				ULearningAgentsActions::MakeFloatAction(InActionObject,
					ThrottleBrakeValue, ThrottleBrakeFloatActionScale, UEpicLearningNamesBPFL::CarStateNameThrottleBrake()),
			});
}
