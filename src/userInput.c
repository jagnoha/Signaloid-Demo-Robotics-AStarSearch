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

#include "userInput.h"
#include "constants.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <uncertain.h>

extern char * optarg;
extern int    opterr;

typedef enum
{
	kMaximumLineLength = 1024,
} UserInputConstants;

static void
printUsage(void)
{
	printf("Example implementing the A* search algorithm.\n\n");
	printf("Usage is:\n");
	printf("a-star-search [-i file] [-n number of nodes] [-s start node] [-e end node] [-f "
	       "heuristic uncrtainty] [-w edge uncertainty] [-h]\n"
	       "\n"
	       "-i file: path to input data CSV file. (Default: \"input.csv\")\n"
	       "\n"
	       "-n number of nodes: number of nodes/vertices in the graph. (Default: 6)\n"
	       "\n"
	       "-s start node: index of start node (zero indexed). (Default: 0)\n"
	       "\n"
	       "-e end node: index of destination node (zero indexed). (Default: 5)\n"
	       "\n"
	       "-f heuristic standard deviation coefficient: add Gaussian noise propotional to the "
	       "heuristic value. (Default: 0.1)\n"
	       "\n"
	       "-w edge weight standard deviation coefficient: add Gaussian noise propotional to "
	       "the edge weight. (Default: 0.05)\n"
	       "\n"
	       "-h: display this help message.\n"
	       "\n");
}

static int
parseInt(char * arg, const char * opt)
{
	const int base = 10;
	errno = 0;
	int i = (int)strtol(arg, NULL, base);
	if (errno != 0)
	{
		perror(opt);
		exit(kReturnCodeError);
	}
	return i;
}

double
parseDouble(char * arg, const char * opt)
{
	errno = 0;
	double d = strtod(arg, NULL);
	if (errno != 0)
	{
		perror(opt);
		exit(kReturnCodeError);
	}
	return d;
}


static int
readCSVLine(FILE * stream, double * const buffer, const size_t numberOfItems)
{
	char   charBuffer[kMaximumLineLength];
	char * charBufferPointer;
	char * returnPointer;

	returnPointer = fgets(charBuffer, sizeof(charBuffer) / sizeof(char), stream);
	if (returnPointer == NULL)
	{
		/*
		 *	Failed to read line from stream.
		 */
		return kReturnCodeError;
	}

	charBufferPointer = charBuffer;

	for (size_t i = 0; i < numberOfItems; i++)
	{
		int       bytesConsumed;
		const int returnValue =
			sscanf(charBufferPointer, "%lf,%n", &buffer[i], &bytesConsumed);
		if (returnValue == EOF || returnValue == 0)
		{
			return kReturnCodeError;
		}
		charBufferPointer += bytesConsumed;
	}

	return kReturnCodeOK;
}

int
checkArgumentValidity(const CommandLineArguments * const arguments)
{
	if (arguments->numberOfNodes < 2)
	{
		printf("Error: please specify a graph with at least two vertices.\n");
		return kReturnCodeError;
	}

	if (arguments->numberOfNodes > kMaximumNumberOfNodes)
	{
		printf("Error: too many graph vertices (specified %zu, out of a maximum of %zu).\n",
		       arguments->numberOfNodes,
		       (size_t)kMaximumNumberOfNodes);
		return kReturnCodeError;
	}

	if (arguments->startNode >= arguments->numberOfNodes)
	{
		printf("Error: start node index (%zu) is out of range (0 to %zu)\n",
		       arguments->startNode,
		       arguments->numberOfNodes - 1);
		return kReturnCodeError;
	}

	if (arguments->endNode >= arguments->numberOfNodes)
	{
		printf("Error: end node index (%zu) is out of range (0 to %zu)\n",
		       arguments->endNode,
		       arguments->numberOfNodes - 1);
		return kReturnCodeError;
	}

	return kReturnCodeOK;
}

int
getCommandLineArguments(int argc, char * argv[], CommandLineArguments * const arguments)
{
	int opt;

	if (argv == NULL || arguments == NULL)
	{
		printf("Error: null pointer passed to getCommandLineArguments()\n");
		return kUserInputError;
	}

	opterr = 0;

	while ((opt = getopt(argc, argv, ":i:n:s:e:f:w:h")) != EOF)
	{
		switch (opt)
		{
		case 'i':
			arguments->inputFilePath = optarg;
			break;
		case 'n':
			arguments->numberOfNodes = parseInt(optarg, "Option '-n'");
			break;
		case 's':
			arguments->startNode = parseInt(optarg, "Option '-s'");
			break;
		case 'e':
			arguments->endNode = parseInt(optarg, "Option '-e'");
			break;
		case 'f':
			arguments->heuristicStdCoeff = parseDouble(optarg, "Option '-d'");
			break;
		case 'w':
			arguments->edgeWeigthStdCoeff = parseDouble(optarg, "Option '-w'");
			break;
		case 'h':
			printUsage();
			return kUserInputHelpMessage;
		default:
			printf("Error: invalid option -%c\n", opt);
			printUsage();
			return kUserInputError;
		}
	}

	if (checkArgumentValidity(arguments) != kReturnCodeOK)
	{
		return kUserInputError;
	}

	return kUserInputOK;
}

int
readFromCSVFile(Graph * const graph, const CommandLineArguments * const arguments)
{
	FILE * stream;

	if (graph == NULL || arguments == NULL || arguments->inputFilePath == NULL)
	{
		printf("Error: null pointer passed to readFromCSVFile()\n");
		return kReturnCodeError;
	}

	stream = fopen(arguments->inputFilePath, "r");

	if (stream == NULL)
	{
		printf("Error: could not open file at path: %s\n", arguments->inputFilePath);
		return kReturnCodeError;
	}

	/*
	 *	First, get node weights
	 */
	if (readCSVLine(stream, graph->heuristicValues, arguments->numberOfNodes) != kReturnCodeOK)
	{
		printf("Error: failed to read line from file %s\n", arguments->inputFilePath);
		goto EXIT_FCLOSE_ERROR;
	}

	/*
	 *	Then get adjacency matrix.
	 */
	for (size_t i = 0; i < arguments->numberOfNodes; i++)
	{
		if (readCSVLine(stream, graph->adjacencyMatrix[i], arguments->numberOfNodes) !=
		    kReturnCodeOK)
		{
			printf("Error: failed to read line from file %s\n",
			       arguments->inputFilePath);
			goto EXIT_FCLOSE_ERROR;
		}
	}

	fclose(stream);

	/*
	 *	Insert uncertainty information.
	 */
	for (size_t i = 0; i < arguments->numberOfNodes; i++)
	{
		if (arguments->heuristicStdCoeff > 0.0)
		{
			const double heuristicValue = graph->heuristicValues[i];
			graph->heuristicValues[i] = libUncertainDoubleGaussDist(
				heuristicValue,
				arguments->heuristicStdCoeff * heuristicValue);
		}

		if (arguments->edgeWeigthStdCoeff > 0.0)
		{
			for (size_t j = 0; j < arguments->numberOfNodes; j++)
			{
				const double edgeWeight = graph->adjacencyMatrix[i][j];
				graph->adjacencyMatrix[i][j] = libUncertainDoubleGaussDist(
					edgeWeight,
					arguments->edgeWeigthStdCoeff * edgeWeight);
			}
		}
	}

	return kReturnCodeOK;

EXIT_FCLOSE_ERROR:
	fclose(stream);
	return kReturnCodeError;
}
