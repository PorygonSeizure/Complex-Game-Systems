#include "NeuralNetwork.h"
#include "Gizmos.h"
#include "matrix.h"

using glm::vec4;
using glm::vec3;
using glm::vec2;
using std::vector;

//colours used for displaying the classifications that the neural network as learnt
const vec4 colours[] = { vec4(0, 0, 0, 0.5), vec4(1, 0, 0, 0.5), vec4(0, 1, 0, 0.5), vec4(0, 0, 1, 0.5) };

//Constructor for a neural network
NeuralNetwork::NeuralNetwork(int numberInputs, int numberHidden, int numberOutputs) : 
	//keep local copies of the network dimensions
	m_numberInputs(numberInputs), 
	m_numberHidden(numberHidden), 
	m_numberOutputs(numberOutputs)
{
	//create hidden layer matrix
	m_hiddenLayer = new Matrix(m_numberInputs, m_numberHidden);
	m_hiddenLayer->random(); //fills matrix with random floats between 0 and 1
	*m_hiddenLayer = *m_hiddenLayer - 0.5f; //range is now -.5 to .5
	*m_hiddenLayer = *m_hiddenLayer * 2; //range is now -1 to 1
}

NeuralNetwork::~NeuralNetwork()
{
	delete(m_hiddenLayer);
	delete(m_outputLayer);
}

//sigmoidal function
float ActivationFunction(float x)
{
	return 1.f / (1.f + exp(-x));
}

//applies a function to all the elements in an array
void NeuralNetwork::ApplyActivationFunction(Matrix* matrix)
{
	for (int i = 0; i < matrix->dim1(); i++)
	{
		for (int j = 0; j < matrix->dim2(); j++)
		{
			(*matrix)(i, j) = ActivationFunction((float)(*matrix)(i, j));
		}
	}
}

void NeuralNetwork::TrainNetwork(vector<vec3>& data)
{
	//first we need to convert out history vector into a form we can work with.  Annoying but necessary..
	Matrix input(data.size(), m_numberInputs); //we need to normalize these values
	Matrix targetOutput(data.size(), m_numberOutputs);
	//we need a matrix which just has the input values in it and one which just has the output values
	for (unsigned int i = 0; i < data.size(); i++)
	{
		input(i, 0) = data[i].x;
		input(i, 1) = data[i].y;
		//now we need to convert our training classification into an output node index.
		//a training value of 0 means the node with index 1 is set to 1 and all others are 0, 
		//a value of 1 means that output index 1 is 1 and all others are 0, etc...
		for (int index = 0; index < m_numberOutputs; index++)
		{
			if (data[i].z == index)  //if this is the index of our training data then set this node to 1
			{
				targetOutput(i, index) = 1;
			}
			else //else set it to zero
			{
				targetOutput(i, index) = 0;
			}
		}
	}
	//feed the data forward through the input layers
	Matrix hiddenOutput = Feedforward(&input);
	Matrix output1 = pseudoinverse(hiddenOutput) * targetOutput;
	//if we have already trained the network delete the old output layer
	if (m_outputLayer != NULL)
	{
		delete(m_outputLayer);
	}
	m_outputLayer = new Matrix(output1); //and add the trained outputlayer
}

void NeuralNetwork::SetUpDebugRender(int numberOfSamples)
{
	m_numberOfSamples = numberOfSamples;
	for (float j = 0.f; j < 1.f; j += 1.f / (float)numberOfSamples)
	{
		for (float i = 0.f; i < 1.f; i += 1.f / (float)numberOfSamples)
		{
			float value = 0.f;
			m_testData.push_back(vec3(i, j, value));
		}
	}
}

void NeuralNetwork::RenderDebug(vec2 screenOffset, int displaySize, vector<vec3>& memory)
{
	TestNetwork(m_testData);
	float elementPixelSize = (float)displaySize / (float)m_numberOfSamples;
	vec2 extent = vec2(elementPixelSize / 2.f, elementPixelSize / 2.f);
	for (auto& element : m_testData)
	{
		int colourIndex = (int)element.z; //classification is returned in Z.
		colourIndex = glm::clamp(colourIndex, 0, 4);
		vec4 colour = colours[colourIndex];
		vec2 screenPos(element.x  * displaySize, element.y * displaySize );
		Gizmos::Add2DAABBFilled(screenOffset + screenPos, extent, colour);
	}
	for (auto& element : memory)
	{
		vec4 colour = (int)element.z % 2 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 0);
		vec2 screenPos(element.x  * displaySize, element.y * displaySize);
		Gizmos::Add2DCircle(screenOffset + screenPos, elementPixelSize / 2.f, 10, colour);
	}
}

//feeds inputs to the neural network through the hidden layer.
Matrix NeuralNetwork::Feedforward(Matrix* input)
{
	Matrix hiddenOutput = *input * *m_hiddenLayer;
	ApplyActivationFunction(&hiddenOutput);
	return hiddenOutput;
}

//runs one pair of x,y coordinates through the trained network and returns the classied value in the z component
void NeuralNetwork::TestNetwork(vec3& data)
{
	Matrix* input = new Matrix(1, 2); //we need to normalize these values
	(*input)(0, 0) = data.x;
	(*input)(0, 1) = data.y;
	Matrix hiddenOutput = Feedforward(input);
	Matrix output = hiddenOutput * *m_outputLayer;
	data.z = (float)Classify(output.get_row(0));
	delete(input);
}

//runs a set of x,y coordinates through the trained network and returns the classied values for each in the z component
void NeuralNetwork::TestNetwork(vector<vec3>& data)
{
	//first we need to convert out history vector into a form we can work with.  Annoying but necessary..
	Matrix* input = new Matrix(data.size(), 2); //we need to normalize these values
	//we need a matrix which just has the input values in it and one which just has the output values
	int rows = input->dim1();
	for (int i = 0; i < rows; i++)
	{
		(*input)(i, 0) = data[i].x;
		(*input)(i, 1) = data[i].y;
	}
	//feed the data forward through our input layer
	Matrix hiddenOutput = Feedforward(input);
	//finally we "test" our output layer
	Matrix output = hiddenOutput * *m_outputLayer;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		data[i].z = (float)Classify(output.get_row(i));
	}
	delete(input);
}

int NeuralNetwork::Classify(Matrix& data)
{
	//very simple classifier, returns the index of the output node which has the highest value
	int bestIndex = 0;
	for (int index = 1; index < data.dim2(); index++)
	{
		if (data(0, index) > data(0, bestIndex))
		{
			bestIndex = index;
		}
	}
	return bestIndex;
}