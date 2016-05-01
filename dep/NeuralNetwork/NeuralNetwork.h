#pragma once

#include <vector>
#include "glm/glm.hpp"

class Matrix;

class NeuralNetwork
{
	int _numberInputs;
	int _numberHidden;
	int _numberOutputs;
	int _numberOfSamples;
	Matrix *hiddenLayer;
	Matrix *outputLayer = NULL;
	std::vector<glm::vec3> testData; //This matrix will contain the x,y coordinates for displaying the NN representation of the scene
	void applyActivationFunction(Matrix*);
public:
	NeuralNetwork(int numberInputs, int numberHidden, int numberOutputs);
	~NeuralNetwork();
	void trainNetwork(std::vector<glm::vec3>&);
	void testNetwork(std::vector<glm::vec3>&);
	void testNetwork(glm::vec3&);
	int classify(Matrix& m);
	Matrix feedforward(Matrix* input);
	void setUpDebugRender(int sampleSize);
	void renderDebug(glm::vec2, int displaySize, std::vector<glm::vec3>&);
};