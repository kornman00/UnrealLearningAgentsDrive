// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "EpicLearningNeuralNetworks.generated.h"

class ULearningAgentsNeuralNetwork;

USTRUCT(BlueprintType)
struct EPICLEARNING_API FEpicLearningNeuralNetworks
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Neural Networks")
	ULearningAgentsNeuralNetwork* EncoderNeuralNetwork;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Neural Networks")
	ULearningAgentsNeuralNetwork* PolicyNeuralNetwork;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Neural Networks")
	ULearningAgentsNeuralNetwork* DecoderNeuralNetwork;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Neural Networks")
	ULearningAgentsNeuralNetwork* CriticNeuralNetwork;
};
