//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#ifndef EANN_SIMPLE_EVOLUTION_MANAGER_H
#define EANN_SIMPLE_EVOLUTION_MANAGER_H

#include "genetic_algorithm.h"
#include "agent_controller.h"

// Singleton class for managing the evolutionary processes.
class EvolutionManager {
public:

    static EvolutionManager *getInstance();
    ~EvolutionManager();

    static int getGenerationCount();
    static void startEvolution();
    static void restartAlgorithm(float wait);
    static GeneticAlgorithm *getGeneticAlgorithm();

    static void writeStatisticsFileStart();
    static void writeStatisticsToFile();
    static void checkForTrackFinished();
    static bool checkGenerationTermination();
    static void onGATermination();
    static void startEvaluation(std::list<Genotype> currentPopulation);
    static void onAgentDied();
    static std::list<Genotype> *remainderStochasticSampling(std::list<Genotype> currentPopulation);
    static std::list<Genotype> *randomRecombination(std::list<Genotype> intermediatePopulation, int newPopulationSize);
    static void mutateAllButBestTwo(std::list<Genotype> newPopulation);
    static void mutateAll(std::list<Genotype> newPopulation);
    static void evalFinished();

    // The amount of agents that are currently alive.
    int agentsAliveCount = 0;

    // Event for when all agents have died.
    Event allAgentsDied;

private:

    static EvolutionManager *instance;
    // private constructor to prevent instancing.
    EvolutionManager();

    // Whether or not the results of each generation shall be written to file.
    bool saveStatistics = false;
    std::string statisticsFileName;
    std::ofstream statisticsFile;

    // How many of the first to finish the course should be saved to file
    int saveFirstNGenotype = 0;
    int genotypesSaved = 0;

    // Population size
    int populationSize = 30;

    // After how many generations should the genetic algorithm be restarted (0 for never)
    int restartAfter = 100;

    // Whether to use elitist selection or remainder stochastic sampling
    bool elitistSelection = false;

    // Topology of the agent's FNN
    int* ffnTopology;

    // The current population agents.
    std::list<std::shared_ptr<Agent>> agents;

    // The current population agents.
    std::list<std::shared_ptr<AgentController>> agentControllers;

    GeneticAlgorithm *geneticAlgorithm;
};

#endif //EANN_SIMPLE_EVOLUTION_MANAGER_H
