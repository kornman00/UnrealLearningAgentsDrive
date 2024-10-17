// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <LearningAgentsManager.h>

#include "EpicLearningCarAgentsManagerComponent.generated.h"

UCLASS(Blueprintable)
class EPICLEARNING_API UEpicLearningCarAgentsManagerComponent : public ULearningAgentsManager
{
	GENERATED_BODY()

public:
	UEpicLearningCarAgentsManagerComponent();

	virtual void PostInitProperties() override;
};
