#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include "ArgumentManager.h"
using namespace std;


class Graph
{
protected:
	int maxVertices; //maximum vertices
	int currNumVert; //current vertices
	list<int> *graph; // array to create the adjacency lists

public:

	Graph()
	{
		maxVertices = 0;
		currNumVert = 0;
	}

	Graph(int x)
	{
		maxVertices = x;
		currNumVert = 0;
		graph = new list<int>[x];
	}

	bool isEmpty() const
	{
		return currNumVert == 0;
	}

	~Graph()
	{
		delete[] graph;
	}

	void destroyGraph()
	{
		for (int i = 0; i < currNumVert; i++)
		{
			graph[i].clear();
		}

		currNumVert = 0;
	}

	void depthFT(int v, bool isVisited[])
	{
		isVisited[v] = true;

		list<int>::iterator graphIt;

		// for each v adjacent to v. 
		// for loop to traverse a linked list (adjacency list) to which the pointer graph[v] points.
		for (graphIt = graph[v].begin(); graphIt != graph[v].end(); ++graphIt)
		{
			int w = *graphIt; // returns the label of the vertex, adjacent to the vertex v, to which graphIt points.
			if (!isVisited[w])
			{
				depthFT(w, isVisited);
			}
		}
	}


	void depthFirstTraversal(int& gCounter) // performs a depth-first traversal of the entire graph
	{
		// pointer to create the array to keep track of the visited vertices
		bool *isVisited, *lastPos;

		isVisited = new bool[currNumVert];
		lastPos = new bool[currNumVert];

		for (int i = 0; i < currNumVert; i++)
		{
			isVisited[i] = false;
			lastPos[i] = false;
		}

		// for each vertices that isnt visited, do a depth first traversal
		for (int i = 0; i < currNumVert; i++)
		{
			if (!isVisited[i])
			{
				depthFT(i, isVisited);
				gCounter++;
			}
		}

		delete[]isVisited;
		delete[]lastPos;
	}

	void implementGraph(ifstream &readFile, const int& x) // this function creates the graph
	{
		string edge;
		int vert, adjVert;

		if (currNumVert != 0)
		{
			destroyGraph();
		}

		currNumVert = x;

		if (!(readFile.is_open()))
		{
			cout << "ERROR: File not found!" << endl;
			return;
		}

		vert = 0;

		while (getline(readFile, edge) && !edge.empty())
		{

			stringstream stream(edge);

			while (stream >> adjVert)
			{
				graph[vert].push_back(adjVert);
			}

			vert++;
		}
	}


};

class MinSpanTree : public Graph // class to find the minimum spanning tree for the graph
{
protected:
	int sourceVar;
	double **totalWeights;
	int *totalEdges;
	double *totalEdgeWeights;
public:

	MinSpanTree()
	{
		totalWeights = 0;
		totalEdges = 0;
		totalEdgeWeights = 0;
	}

	MinSpanTree(int x) : Graph(x)
	{
		totalWeights = new double*[x];

		for (int i = 0; i < x; i++)
		{
			totalWeights[i] = new double[x];
		}

		totalEdges = new int[x];
		totalEdgeWeights = new double[x];
	}


	~MinSpanTree()
	{
		for (int i = 0; i < currNumVert; i++)
		{
			delete[] totalWeights[i];
		}

		delete[] totalWeights;
		delete[] totalEdges;
		delete totalEdgeWeights;
	}


	void depthTraverse(int cluster, ofstream& writeFile) // this function will traverse through the graph in order to determine which nodes with its weights are connected
	{				
		int clusterCounter = 0;

		while (clusterCounter != cluster)
		{
			clusterCounter = 0;
			depthFirstTraversal(clusterCounter);

			if (clusterCounter != cluster)
			{
				double maxWeight = 0;

				for (int i = 0; i < currNumVert; i++) {
					for (int j = 0; j < currNumVert; j++) {
						if (maxWeight < totalWeights[i][j]) {
							maxWeight = totalWeights[i][j];
						}
					}
				}


				for (int i = 0; i < currNumVert; i++) {
					for (int j = 0; j < currNumVert; j++) {
						if (maxWeight == totalWeights[i][j]) {
							totalWeights[i][j] = -999;
							graph[i].remove(j);
							graph[j].remove(i);
						}
					}
				}
			}
			else
			{
				clusterCounter = 0;

				bool *isVisited = new bool[currNumVert];
				bool *lastPos = new bool[currNumVert];

				for (int i = 0; i < currNumVert; i++)
				{
					isVisited[i] = false;
					lastPos[i] = false;
				}

				for (int i = 0; i < currNumVert; i++) {
					if (!isVisited[i]) {
						depthFT(i, isVisited);
						clusterCounter++;

						for (int i = 0; i < currNumVert; i++)
						{
							if (isVisited[i] && !lastPos[i])
							{
								cout << i << " ";
								writeFile << i << " ";
							}
						}

						cout << endl;
						writeFile << endl;

						for (int i = 0; i < currNumVert; i++)
						{
							if (!lastPos[i] && isVisited[i])
							{
								lastPos[i] = isVisited[i];
							}
						}
					}
				}
				delete[] isVisited;
				delete[] lastPos;
			}
		}

	}


	void createWeight(ifstream& readFile) // this function will add the weights to the graph
	{		 
		int x = 0;
		string edge;
		while (getline(readFile, edge) && !edge.empty())
		{
			int j = 0;
			int z;
			stringstream split(edge);
			while (split >> z)
			{
				totalWeights[x][j] = z;
				j++;
			}
			x++;
		}
	}

};


int main(int argc, char *argv[])
{
	string bar;
	int sizeOfList = 0, weight = 0 , nGroup;

	ArgumentManager am(argc, argv);
	string infilename = am.get("A");
	string outfilename = am.get("C");

	ifstream readFile(infilename);
	ofstream writeFile(outfilename);


	while (getline(readFile, bar) && !bar.empty())
	{
		sizeOfList++;
	}

	while (getline(readFile, bar) && !bar.empty())
	{
		weight++;
	}

	if (sizeOfList != weight)
	{
		writeFile.close();
	}
	else
	{
		
		MinSpanTree minSpanGraph(sizeOfList);

		readFile.seekg(0);

		minSpanGraph.implementGraph(readFile, sizeOfList);

		minSpanGraph.createWeight(readFile);

		readFile >> nGroup;

		if (nGroup < 1 || nGroup > sizeOfList)
		{
			writeFile.close();
		}
		else
		{
			minSpanGraph.depthTraverse(nGroup, writeFile);
			readFile.close();
			writeFile.close();
		}
	}

	return 0;
}
