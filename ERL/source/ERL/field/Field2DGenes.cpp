#include <erl/field/Field2DGenes.h>

#include <erl/field/Field2DEvolverSettings.h>

#include<neat/Evolver.h>

using namespace erl;

void Field2DGenes::initialize(size_t numInputs, size_t numOutputs, const neat::EvolverSettings* pSettings, const std::vector<float> &functionChances, neat::InnovationNumberType &innovationNumber, std::mt19937 &generator) {
	const Field2DEvolverSettings* pF2DSettings = static_cast<const Field2DEvolverSettings*>(pSettings);

	_connectionResponseSize = 1;
	_nodeOutputSize = 1;

	_connectionUpdateGenotype.initialize(_nodeOutputSize, _connectionResponseSize, pSettings, functionChances, innovationNumber, generator);
	_activationUpdateGenotype.initialize(_connectionResponseSize, _nodeOutputSize, pSettings, functionChances, innovationNumber, generator);
	_typeSetGenotype.initialize(2, 1, pSettings, functionChances, innovationNumber, generator);
	_encoderGenotype.initialize(1, _connectionResponseSize, pSettings, functionChances, innovationNumber, generator);
	_decoderGenotype.initialize(_nodeOutputSize, 1, pSettings, functionChances, innovationNumber, generator);
}

void Field2DGenes::crossover(const neat::EvolverSettings* pSettings, const std::vector<float> &functionChances, const Evolvable* pOtherParent, Evolvable* pChild, float fitnessForThis, float fitnessForOtherParent, neat::InnovationNumberType &innovationNumber, std::mt19937 &generator) {
	const Field2DEvolverSettings* pF2DSettings = static_cast<const Field2DEvolverSettings*>(pSettings);
	const Field2DGenes* pF2DOtherParent = static_cast<const Field2DGenes*>(pOtherParent);
	Field2DGenes* pF2DChild = static_cast<Field2DGenes*>(pChild);

	std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
	
	pF2DChild->_connectionResponseSize = dist01(generator) < 0.5f ? _connectionResponseSize : pF2DOtherParent->_connectionResponseSize;
	pF2DChild->_nodeOutputSize = dist01(generator) < 0.5f ? _nodeOutputSize : pF2DOtherParent->_nodeOutputSize;

	_connectionUpdateGenotype.crossover(pSettings, functionChances, &pF2DOtherParent->_connectionUpdateGenotype, &pF2DChild->_connectionUpdateGenotype, fitnessForThis, fitnessForOtherParent, innovationNumber, generator);
	_activationUpdateGenotype.crossover(pSettings, functionChances, &pF2DOtherParent->_activationUpdateGenotype, &pF2DChild->_activationUpdateGenotype, fitnessForThis, fitnessForOtherParent, innovationNumber, generator);
	_typeSetGenotype.crossover(pSettings, functionChances, &pF2DOtherParent->_typeSetGenotype, &pF2DChild->_typeSetGenotype, fitnessForThis, fitnessForOtherParent, innovationNumber, generator);
	_encoderGenotype.crossover(pSettings, functionChances, &pF2DOtherParent->_encoderGenotype, &pF2DChild->_encoderGenotype, fitnessForThis, fitnessForOtherParent, innovationNumber, generator);
	_decoderGenotype.crossover(pSettings, functionChances, &pF2DOtherParent->_decoderGenotype, &pF2DChild->_decoderGenotype, fitnessForThis, fitnessForOtherParent, innovationNumber, generator);

	pF2DChild->_connectionUpdateGenotype.setNumInputs(_nodeOutputSize);
	pF2DChild->_connectionUpdateGenotype.setNumOutputs(_connectionResponseSize, pSettings->_minBias, pSettings->_maxBias, functionChances, generator);

	pF2DChild->_activationUpdateGenotype.setNumInputs(_connectionResponseSize);
	pF2DChild->_activationUpdateGenotype.setNumOutputs(_nodeOutputSize, pSettings->_minBias, pSettings->_maxBias, functionChances, generator);

	pF2DChild->_encoderGenotype.setNumOutputs(_connectionResponseSize, pSettings->_minBias, pSettings->_maxBias, functionChances, generator);

	pF2DChild->_decoderGenotype.setNumInputs(_nodeOutputSize);
}

void Field2DGenes::mutate(const neat::EvolverSettings* pSettings, const std::vector<float> &functionChances, neat::InnovationNumberType &innovationNumber, std::mt19937 &generator) {
	const Field2DEvolverSettings* pF2DSettings = static_cast<const Field2DEvolverSettings*>(pSettings);

	_connectionUpdateGenotype.mutate(pSettings, functionChances, innovationNumber, generator);
	_activationUpdateGenotype.mutate(pSettings, functionChances, innovationNumber, generator);
	_typeSetGenotype.mutate(pSettings, functionChances, innovationNumber, generator);
	_encoderGenotype.mutate(pSettings, functionChances, innovationNumber, generator);
	_decoderGenotype.mutate(pSettings, functionChances, innovationNumber, generator);

	std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

	if (dist01(generator) < pF2DSettings->_addConnectionResponseChance)
		_connectionResponseSize++;

	if (dist01(generator) < pF2DSettings->_addNodeOutputChance)
		_nodeOutputSize++;

	_connectionUpdateGenotype.setNumInputs(_nodeOutputSize);
	_connectionUpdateGenotype.setNumOutputs(_connectionResponseSize, pSettings->_minBias, pSettings->_maxBias, functionChances, generator);

	_activationUpdateGenotype.setNumInputs(_connectionResponseSize);
	_activationUpdateGenotype.setNumOutputs(_nodeOutputSize, pSettings->_minBias, pSettings->_maxBias, functionChances, generator);

	_encoderGenotype.setNumOutputs(_connectionResponseSize, pSettings->_minBias, pSettings->_maxBias, functionChances, generator);

	_decoderGenotype.setNumInputs(_nodeOutputSize);
}

float Field2DGenes::getSimilarity(const neat::EvolverSettings* pSettings, const std::vector<float> &functionChances, const Evolvable* pOther) {
	const Field2DEvolverSettings* pF2DSettings = static_cast<const Field2DEvolverSettings*>(pSettings);
	const Field2DGenes* pF2DOther = static_cast<const Field2DGenes*>(pOther);

	return std::abs(_connectionResponseSize - pF2DOther->_connectionResponseSize) * pF2DSettings->_connectionReponseDifferenceFactor +
		std::abs(_nodeOutputSize - pF2DOther->_nodeOutputSize) * pF2DSettings->_nodeOutputSizeDifferenceFactor +
		_connectionUpdateGenotype.getSimilarity(pSettings, functionChances, &pF2DOther->_connectionUpdateGenotype) +
		_activationUpdateGenotype.getSimilarity(pSettings, functionChances, &pF2DOther->_activationUpdateGenotype) +
		_typeSetGenotype.getSimilarity(pSettings, functionChances, &pF2DOther->_typeSetGenotype) +
		_encoderGenotype.getSimilarity(pSettings, functionChances, &pF2DOther->_encoderGenotype) +
		_decoderGenotype.getSimilarity(pSettings, functionChances, &pF2DOther->_decoderGenotype);
}