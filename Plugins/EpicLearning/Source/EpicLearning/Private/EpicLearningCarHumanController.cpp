// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarHumanController.h"

#include "EpicLearningWheeledVehiclePawn.h"

#include <LearningAgentsManager.h>

void UEpicLearningCarHumanController::EvaluateAgentSteeringAndThrottleBrake(
	const int32 AgentId,
	float& OutSteering,
	float& OutThrottleBrake) const
{
	// https://dev.epicgames.com/community/learning/courses/kRm/unreal-engine-learning-agents-5-4/6deP/unreal-engine-learning-to-drive-imitation-5-4#recordinghumandata

	const auto* AgentActor = Cast<AEpicLearningWheeledVehiclePawn>(Manager->GetAgent(AgentId));

	OutSteering = AgentActor->GetSteeringAction();
	OutThrottleBrake = AgentActor->GetThrottleBrakeAction();
}
