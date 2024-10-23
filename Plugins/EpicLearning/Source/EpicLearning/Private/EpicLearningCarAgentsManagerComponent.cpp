// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarAgentsManagerComponent.h"

UEpicLearningCarAgentsManagerComponent::UEpicLearningCarAgentsManagerComponent()
{
}

void UEpicLearningCarAgentsManagerComponent::PostInitProperties()
{
	MaxAgentNum = 32;
	Super::PostInitProperties();
}
