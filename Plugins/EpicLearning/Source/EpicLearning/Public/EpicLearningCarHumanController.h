// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EpicLearningCarImitationControllerBase.h"

#include <CoreMinimal.h>

#include "EpicLearningCarHumanController.generated.h"

/**
 * Evaluates an agent using state from its AEpicLearningWheeledVehiclePawn which
 * is presumably populated from a human's input.
 */
UCLASS()
class EPICLEARNING_API UEpicLearningCarHumanController : public UEpicLearningCarImitationControllerBase
{
	GENERATED_BODY()
protected:
	virtual void EvaluateAgentSteeringAndThrottleBrake(
		const int32 AgentId,
		float& OutSteering,
		float& OutThrottleBrake) const override;
};
