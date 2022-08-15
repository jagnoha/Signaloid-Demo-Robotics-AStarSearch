/*
 *	Authored 2022, Greg Brooks.
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

#include <stdbool.h>
#include <stddef.h>

typedef enum
{
	kMaximumNumberOfNodes = 64,
} AStarConstants;

typedef struct Graph
{
	double adjacencyMatrix[kMaximumNumberOfNodes][kMaximumNumberOfNodes];
	double heuristicValues[kMaximumNumberOfNodes];
	size_t numberOfNodes;
} Graph;

typedef struct ListEntry
{
	double gScore;
	double fScore;
	size_t previous;
	bool   visited;
} ListEntry;

typedef struct AStarResult
{
	size_t nodes[kMaximumNumberOfNodes];
	size_t numberOfNodes;
	double pathCost;
} AStarResult;

typedef struct AStarInstance
{
	Graph     graph;
	ListEntry nodeList[kMaximumNumberOfNodes];
	size_t    startNode;
	size_t    endNode;
} AStarInstance;

/**
 * @brief Run the A* algorithm to find the shortest path between two nodes.
 *
 * @param AStar     : Pointer to struct storing data structures used by the calculation.
 * @param result    : The shortest path between start and end nodes, represented by a list of nodes.
 * @return int : kReturnCodeOK if successful, else kReturnCodeError
 */
int
AStar(AStarInstance * const instance, AStarResult * const result);
