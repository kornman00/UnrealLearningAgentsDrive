// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <LearningAgentsTrainer.h>

#include "EpicLearningCarsTrainer.generated.h"

class USplineComponent;

// #TODO Rename to UEpicLearningCarsReinforcementTrainer
UCLASS()
class EPICLEARNING_API UEpicLearningCarsTrainer : public ULearningAgentsTrainer
{
	GENERATED_BODY()
public:
	UEpicLearningCarsTrainer();

protected:
	virtual void GatherAgentReward_Implementation(
		float& OutReward,
		const int32 AgentId) override;

	virtual void GatherAgentCompletion_Implementation(
		ELearningAgentsCompletion& OutCompletion,
		const int32 AgentId) override;

	virtual void ResetAgentEpisode_Implementation(const int32 AgentId) override;

public:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Track")
	USplineComponent* TrackSpline = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
	float OffTrackThreshold = 1200.0f;
};
