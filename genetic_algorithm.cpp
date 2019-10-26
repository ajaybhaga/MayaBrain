//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#include "genetic_algorithm.h"

#include <iostream>
#include <string>
#include <list>
#include <cassert>
#include <fstream>      // std::ifstream, std::ofstream
#include <stdlib.h>     /* atoi */
#include <stdio.h>
#include <cstring>


GeneticAlgorithm::GeneticAlgorithm(int genotypeParamCount, int populationSize) {

    this->populationSize = populationSize;
    for (int i = 0; i < populationSize; i++) {
        Genotype* genotype = new Genotype(new float[genotypeParamCount], genotypeParamCount);
        currentPopulation.push_back(*genotype);
    }

    generationCount = 1;
    sortPopulation = true;
    running = false;
}

GeneticAlgorithm::~GeneticAlgorithm() {

    // Deallocate Heap memory
//    delete[] parameters;
}

void GeneticAlgorithm::Start() {
    // Init
    running = true;
    algorithmTerminated = false;
    InitializePopulation(currentPopulation);
    Evaluation(currentPopulation);
}

void GeneticAlgorithm::EvaluationFinished() {
    // Calculate fitness from evaluation
    FitnessCalculationMethod(currentPopulation);

    // Sort population if flag was set
    if (sortPopulation) {
        // TODO sort currentPopulation;
    }

    // Fire fitness calculation finished event
    if (fitnessCalculationFinished) {
        FitnessCalculationMethod(currentPopulation);
    }

    // Check termination criterion
    if (TerminationCriterion != NULL && TerminationCriterion(currentPopulation)) {
        Terminate();
        return;
    }

    // Apply selection
    std::list<Genotype>* intermediatePopulation = Selection(currentPopulation);

    // Apply recombination
    std::list<Genotype>* newPopulation = Recombination(*intermediatePopulation, populationSize);

    // Apply mutation
    Mutation(*newPopulation);

    // Set current population to newly generated one and start evaluation again
    currentPopulation = *newPopulation;
    generationCount++;

    Evaluation(currentPopulation);
}

void GeneticAlgorithm::Terminate() {

    running = false;
    algorithmTerminated = true;
}

void GeneticAlgorithm::DefaultPopulationInitialization(std::list<Genotype> population) {

    // Set parameters to random values in set range
    for (std::list<Genotype>::iterator it = population.begin(); it != population.end(); ++it) {
        /* std::cout << *it; ... */
        it->SetRandomParameters(DefInitParamMin, DefInitParamMax);
    }
}

void GeneticAlgorithm::AsyncEvaluation(std::list<Genotype> currentPopulation) {
    // At this point the async evaluation should be started and after it is finished EvaluationFinished should be called
}

void GeneticAlgorithm::DefaultFitnessCalculation(std::list<Genotype> currentPopulation) {

    // First calculate average evaluation of whole population
    int populationSize = 0;
    float overallEvaluation = 0;
    for (std::list<Genotype>::iterator it = currentPopulation.begin(); it != currentPopulation.end(); ++it) {
        overallEvaluation += it->evaluation;
        populationSize++;
    }

    float averageEvaluation = overallEvaluation / populationSize;

    // Now assign fitness with formula fitness = evaluation / averageEvaluation
    for (std::list<Genotype>::iterator it = currentPopulation.begin(); it != currentPopulation.end(); ++it) {
        it->fitness = it->evaluation / averageEvaluation;
    }
}

std::list<Genotype>* GeneticAlgorithm::DefaultSelectionOperator(std::list<Genotype> currentPopulation) {

    std::list<Genotype>* intermediatePopulation = new std::list<Genotype>();

    // Get first 3 list items (top 3)
    size_t n = 3;
    auto end = std::next(currentPopulation.begin(), std::min(n, currentPopulation.size()));
    std::list<Genotype> b(currentPopulation.begin(), end);
    // Selects best three genotypes of the current population and copies them to the intermediate population.
    for (std::list<Genotype>::iterator it = b.begin(); it != b.end(); ++it) {
        intermediatePopulation->push_back(*it);
    }

    return intermediatePopulation;
}

// Simply crosses the first with the second genotype of the intermediate population until the new population is of desired size.
std::list<Genotype>* GeneticAlgorithm::DefaultRecombinationOperator(std::list<Genotype> intermediatePopulation, int newPopulationSize) {

    if (intermediatePopulation.size() < 2) {
        std::cout << "Intermediate population size must be greater than 2 for this operator.";
        return NULL;
    }

    std::list<Genotype>* newPopulation = new std::list<Genotype>();

    if (newPopulation->size() < newPopulationSize) {

        Genotype* offspring1;
        Genotype* offspring2;

        // Get first 2 list items (top 2)
        size_t n = 2;
        auto end = std::next(intermediatePopulation.begin(), std::min(n, intermediatePopulation.size()));
        std::list<Genotype> b(intermediatePopulation.begin(), end);

        Genotype intermediatePopulation0;
        Genotype intermediatePopulation1;

        int count = 0;
        for (std::list<Genotype>::iterator it = b.begin(); it != b.end(); ++it) {
//            intermediatePopulation->push_back(*it);
            switch (count) {
                case 0:
                    intermediatePopulation0 = *it;
                    break;
                case 1:
                    intermediatePopulation1 = *it;
                    break;

            }
            count++;
        }

        CompleteCrossover(intermediatePopulation0, intermediatePopulation1, DefCrossSwapProb, offspring1, offspring2);

        newPopulation->push_back(*offspring1);
        if (newPopulation->size() < newPopulationSize) {
            newPopulation->push_back(*offspring2);
        }
    }

    return newPopulation;
}

void GeneticAlgorithm::DefaultMutationOperator(std::list<Genotype> newPopulation) {

    // Create the random number generator
    random_d rd{0, 1};

    for (std::list<Genotype>::iterator it = newPopulation.begin(); it != newPopulation.end(); ++it) {

        if (rd() < DefMutationPerc) {
            MutateGenotype(*it, DefMutationProb, DefMutationAmount);
        }
    }
}

void GeneticAlgorithm::CompleteCrossover(Genotype parent1, Genotype parent2, float swapChance, Genotype *offspring1,
                                         Genotype *offspring2) {

    // Create the random number generator
    random_d rd{0, 1};

    // Initialize new parameter vectors
    int parameterCount = parent1.parameterCount;
    float* off1Parameters = new float[parameterCount];
    float* off2Parameters = new float[parameterCount];

    // Iterate over all parameters randomly swapping
    for (int i = 0; i < parameterCount; i++) {

        if (rd() < swapChance) {
            // Swap parameters
            off1Parameters[i] = parent2.getParameter(i);
            off2Parameters[i] = parent1.getParameter(i);
        } else {
            // Dont swap parameters
            off1Parameters[i] = parent1.getParameter(i);
            off2Parameters[i] = parent2.getParameter(i);
        }
    }

    Genotype* offspring1_out = new Genotype(off1Parameters, parameterCount);
    Genotype* offspring2_out = new Genotype(off2Parameters, parameterCount);

    offspring1 = offspring1_out;
    offspring2 = offspring2_out;
}

void GeneticAlgorithm::MutateGenotype(Genotype genotype, float mutationProb, float mutationAmount) {

}

bool GeneticAlgorithm::DefaultTermination(std::list<Genotype> currentPopulation) {
    return false;
}

