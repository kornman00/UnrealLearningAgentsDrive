// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EpicLearningNamesBPFL.generated.h"

UCLASS()
class EPICLEARNING_API UEpicLearningNamesBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
	static FName LearningAgentsManagerName();

public:
	UFUNCTION(BlueprintPure, Category = "Car Agent Actions")
	static FName CarAgentActionNameSteering();
	UFUNCTION(BlueprintPure, Category = "Car Agent Actions")
	static FName CarAgentActionNameThrottleBrake();

public:
	UFUNCTION(BlueprintPure, Category = "Car Agent Rewards")
	static FName CarAgentRewardNameLocationDifferenceAboveThreshold();
	UFUNCTION(BlueprintPure, Category = "Car Agent Rewards")
	static FName CarAgentRewardNameVelocityAlongSpline();

public:
	UFUNCTION(BlueprintPure)
	static FName TrackLookaheadObservationNameLocation();
	UFUNCTION(BlueprintPure)
	static FName TrackLookaheadObservationNameDirection();

	UFUNCTION(BlueprintPure)
	static FName CarObservationNameVelocity();

	UFUNCTION(BlueprintPure)
	static FName CarAgentObservationNameTrackLookahead();
	UFUNCTION(BlueprintPure)
	static FName CarAgentObservationNameSelf();

public:
	static FName DefaultNameForLocationAlongSplineObservation();
	static FName DefaultNameForDirectionAlongSplineObservation();
	static FName DefaultNameForVelocityObservation();
};
