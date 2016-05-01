#pragma once

#include <vector>
#include "glm/glm.hpp"

class Matrix;

class NeuralNetwork
{
public:
	NeuralNetwork(int numberInputs, int numberHidden, int numberOutputs);
	~NeuralNetwork();
	void TrainNetwork(std::vector<glm::vec3>& data);
	void TestNetwork(std::vector<glm::vec3>& data);
	void TestNetwork(glm::vec3& data);
	int Classify(Matrix& data);
	Matrix Feedforward(Matrix* input);
	void SetupDebugRender(int sampleSize);
	void RenderDebug(glm::vec2 screenOffset, int displaySize, std::vector<glm::vec3>& memory);

private:
	int m_numberInputs;
	int m_numberHidden;
	int m_numberOutputs;
	int m_numberOfSamples;
	Matrix* m_hiddenLayer;
	Matrix* m_outputLayer = NULL;
	std::vector<glm::vec3> m_testData; //This matrix will contain the x,y coordinates for displaying the NN representation of the scene

	void ApplyActivationFunction(Matrix* matrix);
};