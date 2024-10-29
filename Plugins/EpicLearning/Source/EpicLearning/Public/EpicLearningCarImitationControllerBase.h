// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <LearningAgentsController.h>

#include "EpicLearningCarImitationControllerBase.generated.h"

UCLASS(Abstract)
class EPICLEARNING_API UEpicLearningCarImitationControllerBase : public ULearningAgentsController
{
	GENERATED_BODY()
public:
	virtual void EvaluateAgentController_Implementation(
		FLearningAgentsActionObjectElement& OutActionObjectElement,
		ULearningAgentsActionObject* InActionObject,
		const ULearningAgentsObservationObject* InObservationObject,
		const FLearningAgentsObservationObjectElement& InObservationObjectElement,
		const int32 AgentId) override;

protected:
	virtual void EvaluateAgentSteeringAndThrottleBrake(
		const int32 AgentId,
		float& OutSteering,
		float& OutThrottleBrake) const PURE_VIRTUAL(,);
public:
	UPROPERTY(EditAnywhere)
	float SteeringFloatActionScale = 1.0f;
	UPROPERTY(EditAnywhere)
	float ThrottleBrakeFloatActionScale = 1.0f;
};
