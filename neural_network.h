//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#ifndef EANN_SIMPLE_NEURAL_NETWORK_H
#define EANN_SIMPLE_NEURAL_NETWORK_H

#include "random_d.h"
#include "neural_layer.h"


class NeuralNetwork {
public:

    NeuralNetwork(int *topology, int numLayers);
    ~NeuralNetwork();

    double *processInputs(double *inputs);
    void setRandomWeights(double minValue, double maxValue);

    // Returns a new NeuralNetwork instance with the same topology and activation functions,
    // but the weights set to their default value.
    NeuralNetwork getTopologyCopy();

    // Deep copy of this NeuralNetwork
    NeuralNetwork deepCopy();

    // Returns a string representing this network in layer order.
    std::string toString();

    // The individual neural layers of this network.
    NeuralLayer **layers;

    // An array of integers representing the node count of each layer of the network from input to output layer.
    int *topology;

    // The amount of overall weights of the connections of this network.
    int weightCount;


    int numLayers;

    // The weights of the connections of this layer to the next layer.
    // e.g. weight [i, j] is the weight of the connection from the i-th weight of this layer to the
    // j-th weight of the next layer.
    double **weights;


private:
};

#endif //EANN_SIMPLE_NEURAL_NETWORK_H
