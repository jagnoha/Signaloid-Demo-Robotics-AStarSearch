/*
 *	Authored 2022, Greg Brooks, Damien Zufferey.
 *
 *	Copyright (c) 2022, Signaloid.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#include "aStar.h"
#include "constants.h"
#include "userInput.h"
#include <stdio.h>

static int
initialiseAStarInstance(
	AStarInstance * const              instance,
	const CommandLineArguments * const arguments)
{
	if (instance == NULL || arguments == NULL)
	{
		printf("Error: null pointer passed to initialiseAStarInstance().\n");
		return kReturnCodeError;
	}

	if (checkArgumentValidity(arguments) != kReturnCodeOK)
	{
		return kReturnCodeError;
	}

	instance->graph.numberOfNodes = arguments->numberOfNodes;
	instance->startNode = arguments->startNode;
	instance->endNode = arguments->endNode;

	if (readFromCSVFile(&(instance->graph), arguments) != kReturnCodeOK)
	{
		return kReturnCodeError;
	}

	return kReturnCodeOK;
}

int
main(int argc, char * argv[])
{
	CommandLineArguments arguments = {
		.inputFilePath = "input.csv",
		.numberOfNodes = 6,
		.startNode = 0,
		.endNode = 5,
		.heuristicStdCoeff = 0.1,
		.edgeWeigthStdCoeff = 0.05,
	};

	AStarInstance aStarInstance;
	AStarResult   aStarResult;

	switch (getCommandLineArguments(argc, argv, &arguments))
	{
	case kUserInputOK:
		/*
		 *	Do nothing.
		 */
		break;
	case kUserInputHelpMessage:
		/*
		 *	Help message has been printed, exit program.
		 */
		return kReturnCodeOK;
	case kUserInputError:
	default:
		/*
		 *	Error encountered, exit program.
		 */
		return kReturnCodeError;
	}

	/*
	 *	Initialise graph structure using user inputs.
	 */
	if (initialiseAStarInstance(&aStarInstance, &arguments) != kReturnCodeOK)
	{
		return kReturnCodeError;
	}

	if (AStar(&aStarInstance, &aStarResult) != kReturnCodeOK)
	{
		return kReturnCodeError;
	}

	printf("Shortest path from vertex %zu to vertex %zu is:\n",
	       aStarInstance.startNode,
	       aStarInstance.endNode);
	printf("%zu", aStarInstance.startNode);

	for (size_t i = 1; i < aStarResult.numberOfNodes; i++)
	{
		const size_t index = aStarResult.numberOfNodes - 1 - i;
		printf("->%zu", aStarResult.nodes[index]);
	}
	printf("\n\n");

	printf("Total path cost: %lf\n", aStarResult.pathCost);

	return kReturnCodeOK;
}
