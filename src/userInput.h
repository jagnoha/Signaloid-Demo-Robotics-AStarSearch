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

#pragma once

#include "aStar.h"

typedef enum
{
	kUserInputOK = 0,
	kUserInputHelpMessage = 1,
	kUserInputError = 2,
} UserInputReturnCodes;

typedef struct CommandLineArguments
{
	char * inputFilePath;
	size_t numberOfNodes;
	size_t startNode;
	size_t endNode;
	double heuristicStdCoeff;
	double edgeWeigthStdCoeff;
} CommandLineArguments;

/**
 *	@brief Check the validity of the arguments in a struct.
 *
 *	@param arguments : Pointer to arguments struct.
 *	@return int : kReturnCodeError if arguments are invalid, else kReturnCodeOK
 */
int
checkArgumentValidity(const CommandLineArguments * const arguments);

/**
 *	@brief Get command line arguments.
 *
 *	@param argc      : Argument count from main().
 *	@param argv      : Argument vector from main().
 *	@param arguments : Pointer to struct to store user inputs.
 *	@return int : kUserInput OK if successfully parsed command line arguments.
 *	              kUserInputHelpMessage if user has requested the help message (i.e. program
 *	              does not need to run) kUserInputError if error encountered.
 */
int
getCommandLineArguments(int argc, char * argv[], CommandLineArguments * const arguments);

/**
 *	@brief Read information about the graph from a CSV file.
 *
 *	@param graph     : Pointer to struct to store input data.
 *	@param arguments : Pointer to struct containing user inputs.
 *	@return int : kReturnCodeOK if successful, else kReturnCodeError
 */
int
readFromCSVFile(Graph * const graph, const CommandLineArguments * const arguments);
