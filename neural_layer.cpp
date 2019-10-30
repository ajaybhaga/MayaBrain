//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#include "neural_layer.h"

NeuralLayer::NeuralLayer(int nodeCount, int outputCount) {

    this->neuronCount = nodeCount;
    this->outputCount = outputCount;

    // Allocate dynamic 2D array for weights
    this->weights = new double*[nodeCount + 1]; // + 1 for bias node
    for (int i = 0; i < nodeCount + 1; i++) {
        this->weights[i] = new double[outputCount];
    }

}

NeuralLayer::~NeuralLayer() {

    for (int i = 0; i < this->neuronCount + 1; i++) {
        delete [] this->weights[i];
    }
    delete [] this->weights;
}


/*void setWeights(double[] weights);
double[] processInputs(double[] inputs);
NeuralLayer deepCopy();
void setRandomWeights(double minValue, double maxValue);
string toString();
*/


// The values to set the weights of the connections from this layer to the next layer.

// The values are ordered in neuron order, i.e. in a layer with two neurons with a next layer of three neurons
// the values [0-2] are the weights from neuron 0 of this layer to neurons 0-2 of the next layer respectively and
// the values [3-5] are the weights from neuron 1 of this layer to neurons 0-2 of the next layer respectively.
void NeuralLayer::setWeights(double *weights) {


    // TODO: assert weights that do not match layer weight count

    // Copy weights from given value array
   int k = 0;
   for (int i = 0; i < this->neuronCount; i++) {
       for (int j = 0; j < this->outputCount; j++) {
           this->weights[i][j] = weights[k++];
       }
   }

}

double *NeuralLayer::processInputs(double *inputs) {

    // TODO: assert inputs where given xValues do not match layer input count

    // Calculate sum for each neuron from weighted inputs and bias
    double *sums = new double[this->outputCount];
    // Add bias (always on) neuron to inputs
    double *biasedInputs = new double[this->neuronCount + 1];
    for (int i = 0; i < this->neuronCount; i++) {
        biasedInputs[i] = 0.0;
    }
    biasedInputs[this->neuronCount] = 1.0;

    for (int j = 0; j < this->outputCount; j++) {
        for (int i = 0; i < this->neuronCount; i++) {
            sums[j] += biasedInputs[i] * this->weights[i][j];
        }
    }

    // Apply activation function to sum, if set
    if (this->neuronActivationFunction) {
        for (int i = 0; i < this->outputCount; i++) {
            sums[i] = this->neuronActivationFunction(sums[i]);
        }
    }

    return sums;
}

NeuralLayer NeuralLayer::deepCopy() {
    return NeuralLayer(0, 0);
}

void NeuralLayer::setRandomWeights(double minValue, double maxValue) {

}

std::string NeuralLayer::toString() {
    return nullptr;
}