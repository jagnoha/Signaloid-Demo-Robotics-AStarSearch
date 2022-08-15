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

#include "aStar.h"
#include "constants.h"
#include <math.h>
#include <stdio.h>

typedef enum
{
	kUnvisitedNodes = 0,
	kAllNodesVisited = 1,
} VisitNodeReturnCodes;

static void
initialiseNodeList(AStarInstance * const instance)
{
	for (size_t i = 0; i < instance->graph.numberOfNodes; i++)
	{
		instance->nodeList[i].gScore = INFINITY;
		instance->nodeList[i].fScore = INFINITY;
		instance->nodeList[i].previous = instance->graph.numberOfNodes;
		instance->nodeList[i].visited = false;
	}

	/*
	 *	Start node
	 */
	instance->nodeList[instance->startNode].gScore = 0;
	instance->nodeList[instance->startNode].fScore =
		instance->graph.heuristicValues[instance->startNode];
	instance->nodeList[instance->startNode].previous = instance->startNode;
}

static size_t
findUnvisitedNodeWithLowestFScore(AStarInstance * const instance)
{
	size_t nextNode = instance->graph.numberOfNodes;
	double nextNodeFScore = INFINITY;

	for (size_t i = 0; i < instance->graph.numberOfNodes; i++)
	{
		if (instance->nodeList[i].visited == false)
		{
			if (instance->nodeList[i].fScore < nextNodeFScore)
			{
				nextNode = i;
				nextNodeFScore = instance->nodeList[i].fScore;
			}
		}
	}

	return nextNode;
}

static void
evaluateNodeNeighbours(const size_t node, AStarInstance * const instance)
{
	const double(*const adjacencyMatrix)[kMaximumNumberOfNodes] =
		instance->graph.adjacencyMatrix;
	const double * const heuristicValues = instance->graph.heuristicValues;
	ListEntry * const    nodeList = instance->nodeList;
	const size_t         numberOfNodes = instance->graph.numberOfNodes;
	const double         previousGScore = nodeList[node].gScore;

	for (size_t i = 0; i < numberOfNodes; i++)
	{
		const double weight = adjacencyMatrix[node][i];

		if (nodeList[i].visited == false && weight != 0.0)
		{
			const double newGScore = previousGScore + weight;

			if (newGScore < nodeList[i].gScore)
			{
				nodeList[i].gScore = newGScore;
				nodeList[i].previous = node;
				nodeList[i].fScore = newGScore + heuristicValues[i];
			}
		}
	}
}

static int
visitNode(AStarInstance * const instance)
{
	const size_t node = findUnvisitedNodeWithLowestFScore(instance);

	if (node == instance->endNode || node == instance->graph.numberOfNodes)
	{
		instance->nodeList[instance->endNode].visited = true;
		return kAllNodesVisited;
	}

	evaluateNodeNeighbours(node, instance);

	instance->nodeList[node].visited = true;

	return kUnvisitedNodes;
}

static void
obtainResult(const AStarInstance * const instance, AStarResult * const result)
{
	size_t index = instance->endNode;

	result->numberOfNodes = 0;
	result->pathCost = instance->nodeList[instance->endNode].gScore;

	while (index != instance->startNode)
	{
		result->nodes[result->numberOfNodes++] = index;
		index = instance->nodeList[index].previous;
	}

	result->nodes[result->numberOfNodes++] = index;
}

int
AStar(AStarInstance * const instance, AStarResult * const result)
{
	int visitNodeReturnCode;

	if (instance == NULL || result == NULL)
	{
		printf("Error: null pointer passed to AStar().\n");
		return kReturnCodeError;
	}

	initialiseNodeList(instance);

	do
	{
		visitNodeReturnCode = visitNode(instance);
	} while (visitNodeReturnCode != kAllNodesVisited);

	obtainResult(instance, result);

	return kReturnCodeOK;
}
