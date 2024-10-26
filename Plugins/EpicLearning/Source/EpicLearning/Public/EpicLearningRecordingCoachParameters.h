// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <LearningAgentsRecorder.h>

#include "EpicLearningRecordingCoachParameters.generated.h"

UCLASS(BlueprintType)
class EPICLEARNING_API UEpicLearningRecordingCoachParameters : public UDataAsset
{
	GENERATED_BODY()
public:
	/** Fed into MakeRecorder */
	UPROPERTY(EditAnywhere, Category = "Learning Settings|Recording")
	FLearningAgentsRecorderPathSettings RecorderPathSettings {};

	/** Agents are reset after this many seconds */
	UPROPERTY(EditAnywhere, Category = "Learning Settings|Recording")
	float AgentsResetFrequency = 20.0f;
};
