// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EpicLearningCarImitationControllerBase.h"

#include <CoreMinimal.h>

#include "EpicLearningCarNonHumanController.generated.h"

/**
 * Evaluates an agent under the influence of a non-human by sampling the Actor state itself.
 */
UCLASS()
class EPICLEARNING_API UEpicLearningCarNonHumanController : public UEpicLearningCarImitationControllerBase
{
	GENERATED_BODY()
protected:
	virtual void EvaluateAgentSteeringAndThrottleBrake(
		const int32 AgentId,
		float& OutSteering,
		float& OutThrottleBrake) const override;
};
