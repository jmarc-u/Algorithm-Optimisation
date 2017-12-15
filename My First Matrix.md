My very first program dealing with matrices and search algorithms.
Inspired from other codes found on the web....

```
/*
* C++ Program to Implement Adjacency Matrix
*/


//#include "Main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <limits>
#include <list>



using namespace std;
#define MAX 20
/*
* Adjacency Matrix Class
*/
class AdjacencyMatrix
{
private:
	int n;
	double **adj;
	//bool *visited;
	int NbCycles;
	static const double INF;
public:
	AdjacencyMatrix::AdjacencyMatrix(int n)
	{
		const double INF = std::numeric_limits<double>::infinity();
		this->n = n;
		//visited = new bool[n];
		this->NbCycles = 0;
		adj = new double*[n];

		for (int i = 0; i < n; i++) 
		{
			adj[i] = new double[n];
			for (int j = 0; j < n; j++)
			{ 
				adj[i][j] = INF;
			}
			adj[i][i] = 0;
		}


/* OLD VERSION
	for (int i = 0; i < n; i++)
		{
			adj[i] = new double[n];
			for (int j = 0; j < n; j++)
			{
				adj[i][j] = 0;
			}
		}
*/

	}




	/*
	* Adding Edge to Graph
	*/
	void AdjacencyMatrix::add_edge(int origin, int destin, double value)
	{
		if (origin > n || destin > n || origin < 1 || destin < 1)
		{
			cout << "Invalid edge!\n";
		}
		else
		{
			adj[origin - 1][destin - 1] = value;
		}
	}
	/*
	* Print the graph
	*/
	void AdjacencyMatrix::display()
	{
		int i, j;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
				cout << adj[i][j] << "  ";
			cout << endl;
		}
	}



	//  check if the graph is stongly connected
	/* 1  procedure DFS(G,v):
      label v as discovered
      for all edges from v to w in G.adjacentEdges(v) do
          if vertex w is not labeled as discovered then
              recursively call DFS(G,w)
			  */

	/*void AdjacencyMatrix::checkStronglyConnected()
	{
		int all_rows_connected = 0; // 0 = FALSE
		int all_col_connected = 0; // 0 = FALSE

		int i, j;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
				cout << adj[i][j] << "  ";
			cout << endl;
		}*/

		// A recursive function to store DFS starting from v
	void AdjacencyMatrix::DFSUtil(int v, bool visited[])
	{
		int j;
		const double INF = std::numeric_limits<double>::infinity();

		// Mark the current node as visited and record it
		visited[v] = true;

		
		
		for (j = 0; j < n; j++)
		{
			// counting the number of cycles. Note due to implementation that a cycle may be counted multiple time
			// In reality a boolean would be more appropriate but we keep a counter to check the code
			if ((adj[v][j] != 0) && (adj[v][j] != INF) && (visited[j]))
				NbCycles++;  // Reviewing it, it doesn't work as you may revisit a node without necessarly having a loop, e.g if only in-degrees to that node
			// Recur for all the vertices adjacent to this vertex unless we are looping back
			if ((adj[v][j] != 0) && (adj[v][j] != INF) && (!visited[j]))
				DFSUtil(j, visited);
		}
			
	}

	// A recursive function to store *reversed* DFS starting from v
	void AdjacencyMatrix::RevDFSUtil(int v, bool visited[])
	{
		int i;
		const double INF = std::numeric_limits<double>::infinity();

		// Mark the current node as visited and record it
		visited[v] = true;

		// Recur for all the vertices adjacent to this vertex

		for (i = 0; i < n; i++)
			if ((adj[i][v] != 0) && (adj[i][v] != INF) && (!visited[i]))
				RevDFSUtil(i, visited);
	}



		// The main function that returns true if graph is strongly connected
		bool AdjacencyMatrix::isSC()
		{
			// St1p 1: Mark all the vertices as not visited (For first DFS)
			bool *visited;                //Declare pointer to type of array
			visited = new bool[n];   //use 'new' to create array of size x//
			// initialize visited
			for (int i = 0; i < n; i++)
				visited[i] = false;

			// Step 2: Do DFS traversal starting from first vertex.
			DFSUtil(0, visited);

			// If DFS traversal doesnít visit all vertices, then return false.
			for (int i = 0; i < n; i++)
				if (visited[i] == false)
				{
					delete[] visited;
					return false;
				}

			// Step 3: Mark all the vertices as not visited (For second DFS)
			for (int i = 0; i < n; i++)
				visited[i] = false;

			// Step 4: Do *reversed* DFS for graph starting from first vertex.
			// Staring Vertex must be same starting point of first DFS
			// The idea is, if every node can be reached from a vertex v, and every node can reach v, 
			// then the graph is strongly connected. In step 2, we check if all vertices are reachable from v. 
			// In step 4, we check if all vertices can reach v (In reversed DFS, if all vertices are reachable from v,
			// then all vertices can reach v in graph). 

			RevDFSUtil(0, visited);

			// If all vertices are not visited in second DFS, then
			// return false
			for (int i = 0; i < n; i++)
				if (visited[i] == false)
				{
					delete[] visited;
					return false;
				}

			delete[] visited;
			return true;
		}



		// Function to calculate in-degree and out-degree of vertex
		void AdjacencyMatrix::calcDegree()
		{
			int i, j;
			const double INF = std::numeric_limits<double>::infinity();
			int *in_degree, *out_degree;                //Declare pointer to type of array
			in_degree = new int[n]; 
			out_degree = new int[n];

			for (i = 0; i < n; i++)
			{
				in_degree[i] = 0;
				out_degree[i] = 0;
			}

			for (i = 0; i < n; i++)
			{
				for (j = 0; j < n; j++)
					if ((adj[i][j] != 0) && (adj[i][j] != INF))
					{
						out_degree[i]++;
						in_degree[j]++;
					}
			}

			// print outcome

			for (i = 0; i < n; i++)
			{
				cout << "Vertex " << i+1 << " has " << in_degree[i] << " in_degree and " << out_degree[i] << " out_degree" << endl;
			}

			delete[] in_degree;
			delete[] out_degree;
		}
	

		// The main function that returns nb of cycles in the graph
		int AdjacencyMatrix::calcCycles()
		{
			int i;
			bool *visited;           //Declare pointer to type of array
			visited = new bool[n];   //use 'new' to create array of size x//
			// initialize visited
			for (int i = 0; i < n; i++)
				visited[i] = false;

			// Reinitialize NbCycles
			NbCycles = 0;

			for (i=0; i < n; i++)
				if (!visited[i])
				{
					DFSUtil(i, visited);
				}

			delete[] visited;

			return NbCycles;

		}


	// Define the destructor.  
	AdjacencyMatrix::~AdjacencyMatrix() {
		// Deallocate the memory that was previously reserved  
		//  for this matrix.  

		for (int i = 0; i < n; i++)
		{
			if (adj[i] != NULL)
			{
				delete adj[i];
			}
		}
		delete adj;
	}


	// A recursive function to store DFS starting from v
	void AdjacencyMatrix::MyDFSUtil(int v, bool visited[])
	{
		int j;
		const double INF = std::numeric_limits<double>::infinity();

		// Mark the current node as visited and record it
		visited[v] = true;

		for (j = 0; j < n; j++)
		{
			// Recur for all the vertices adjacent to this vertex unless we are looping back
			if ((adj[v][j] != 0) && (adj[v][j] != INF) && (!visited[j]))
				DFSUtil(j, visited);
		}

	}



	// An iterative function to store BFS starting from v
	void AdjacencyMatrix::bfs(int v, bool visited[])
	{
		std::list<int> queue;
		queue.push_back(v-1);
		unsigned count = 0;
		int nb;
		int i;
		const double INF = std::numeric_limits<double>::infinity();

		// initialize visited
		for (int i = 0; i < n; i++)
			visited[i] = false;

		while (!queue.empty())
		{
			nb = queue.front();
			
			queue.pop_front();
			if (!visited[nb])
			{
				visited[nb] = true;
				std::cout << nb + 1 << std::endl;
			}
				
			for (i = 0; i < n; i++)
			{
				if ((adj[nb][i] != 0) && (adj[nb][i] != INF) && (!visited[i]))
					queue.push_back(i);
			}
			++count;
		}
	}


	// An iterative function to check if a graph is a DAG (Directed Acyclic Graph)
	void AdjacencyMatrix::IsDAG()
	{
		int *dqueue;
		dqueue = new int[n];   //use 'new' to create array of size n to store node ID//
		//for (int i = 0; i < n; i++)
		//	dqueue[i] = 0;

		int ihead = 0;  // head of the queue. Empty for now
		int itail = -1; // tail of the queue. Empty for now

		int current_top; // current vertex inspected in the queue

		// Calculate in_degree (note out_degree also calculated but not used here)
		int i, j;
		const double INF = std::numeric_limits<double>::infinity();
		int *in_degree;                //Declare pointer to type of array
		in_degree = new int[n];

		for (i = 0; i < n; i++)
		{
			in_degree[i] = 0;
		}

		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
				if ((adj[i][j] != 0) && (adj[i][j] != INF))
				{
					in_degree[j]++;
				}
		}

		
		// Push all vertices that have in-degree zero into the queue

		for (i = 0; i < n; i++)
		{
			if (in_degree[i] == 0)
			{
				++itail;
				dqueue[itail] = i;
			}
		}

		// While the queue is not empty

		while (ihead <= itail)
		{
			// Pop a vertex from the queue
			current_top = dqueue[ihead];
			++ihead;

			// Decrement the in-degree of each neighbor
			
			for (j = 0; j < n; j++) // inspect columns of the Matrix adj for current selected node in the queue
			{
					if ((adj[current_top][j] != 0) && (adj[current_top][j] != INF))
					{
						in_degree[j]--;

						// Those neighbors whose in-degree was decremented to zero are pushed onto the queue
						if (in_degree[j] == 0)
						{
							++itail;
							dqueue[itail] = j;
						}
					}
			}
		}

		// Show the found topological sort
		if (itail > -1)
		{
			cout << "We found following topological sort: ";
			for (i = 0; i < itail; i++) cout << dqueue[i] + 1 << ", "; // +1 as vertex 1 has index 0 
			cout << dqueue[i] + 1 << endl;
		}
		

		if (itail == n - 1)
		{
			cout << "The graph is a DAG";
		}
		else
		{
			cout << "The graph is NOT a DAG";
		}
		cout << endl;
			

		delete[] in_degree;
		delete[] dqueue;
	}

	// ----------------------------------------------------------------------------
	//                Section on DIJKSTRA 
	// ----------------------------------------------------------------------------

	// A utility function to find the vertex with minimum distance value, from
	// the set of vertices not yet included in shortest path tree
	int AdjacencyMatrix::minDistance(double dist[], bool sptSet[])
	{
		// Initialize min value
		const double INF = std::numeric_limits<double>::infinity();
		double min = INF;
		int min_index, v;

		for (v = 0; v < n; v++)
			if (sptSet[v] == false && dist[v] <= min)
				min = dist[v], min_index = v;

		return min_index;
	}

	// A utility function to print the constructed distance array
	void AdjacencyMatrix::printSolution(double dist[], int n)
	{
		int i;
		printf("Vertex   -    Distance from Source\n");
		for (i = 0; i < n; i++)
			printf("%d                %f\n", i, dist[i]);
	}

	// Funtion that implements Dijkstra's single source shortest path algorithm
	// for a graph represented using adjacency matrix representation
//	void AdjacencyMatrix::dijkstra(int graph[V][V], int src)
	void AdjacencyMatrix::dijkstra(int src)
	{
		const double INF = std::numeric_limits<double>::infinity();
		int i;
		int count;
		int v;
			
		double *dist;			// The output array.† dist[i] will hold the shortest
		dist = new double[n];	 // distance from src to i

		bool *sptSet;			// sptSet[i] will true if vertex i is included in shortest
		sptSet = new bool[n];	// path tree or shortest distance from src to i is finalized

		int *Parents;
		Parents = new int[n];	// Store Parent of node (i.e. the previous node in the path) 

						// Initialize all distances as INFINITE and stpSet[] as false
		for (i = 0; i < n; i++)
			dist[i] = INF, sptSet[i] = false;

		// Distance of source vertex from itself is always 0
		dist[src-1] = 0;	// -1 to convert node ID (starting at 1) to array ID (starting at 0)

		// Find shortest path for all vertices
		for (count = 0; count < n - 1; count++)
		{
			// Pick the minimum distance vertex from the set of vertices not
			// yet processed. u is always equal to src in first iteration.
			int u = minDistance(dist, sptSet);

			// Mark the picked vertex as processed
			sptSet[u] = true;

			// Update dist value of the adjacent vertices of the picked vertex.
			for (v = 0; v < n; v++)

				// Update dist[v] only if is not in sptSet, there is an edge from 
				// u to v, and total weight of path from src to† v through u is 
				// smaller than current value of dist[v]
				if (!sptSet[v] && adj[u][v] && dist[u] != INF
					&& dist[u] + adj[u][v] < dist[v])
					dist[v] = dist[u] + adj[u][v];
		}

		// print the constructed distance array
		printSolution(dist, n);

		system("pause");
		delete(dist);
		delete(sptSet);
	}

	// This function constructs a bidrectional matrix, i.e. add the symetrical value for each
	// Only use if you have only one distance between two given vertices, otherwise we will loose some data
	void AdjacencyMatrix::AddBiDir()
	{
		int i, j;
		const double INF = std::numeric_limits<double>::infinity();

		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
				if ((adj[i][j] != 0) && (adj[i][j] != INF))
				{
					adj[j][i] = adj[i][j];
				}
		}
	}

}

;



/*
* Main
*/

/* int main()
{
	
	int nodes, max_edges, origin, destin;
	double weight;
	cout << "Enter number of nodes: ";
	cin >> nodes;
	AdjacencyMatrix am(nodes);
	
	max_edges = nodes * (nodes - 1);
	for (int i = 0; i < max_edges; i++)
	{
		cout << "Enter edge (-1 -1 -1 to exit): ";
		cin >> origin >> destin >> weight;
		if ((origin == -1) && (destin == -1) && (weight == -1))
			break;
		am.add_edge(origin, destin, weight);
	}
	am.display();
	system("pause"); 
	return 0;
}*/

int main()
{
	int nb;
	int i;

	bool *m_visited;           //Declare pointer to type of array
	

	

	// Create graphs given in the above diagrams
	AdjacencyMatrix g1(5); // n must be detected autimatically
	g1.add_edge(1, 2, 4.5);
	g1.add_edge(2, 3, 9.34);
	g1.add_edge(3, 4, 2.1);
	g1.add_edge(4, 1, 33.98);
	g1.add_edge(3, 5, 9.12);
	g1.add_edge(5, 3, 3.1415);
	cout << "Example with Matrix g1\n";
	cout << "----------------------\n";
	cout << endl;
	g1.display();
	cout << endl;
	g1.isSC() ? cout << "It is strongly connected !!\n" : cout << "It is not strongly connected :-(\n"; // Should be YES here !!
	cout << endl;
	g1.calcDegree();
	cout << endl;
	nb = g1.calcCycles();
	nb > 0	? cout << "It has " << nb << " cycles\n" : cout << "It has no cycle\n";
	cout << endl;
	cout << endl;

	AdjacencyMatrix g2(4);
	g2.add_edge(1, 2, 9.99);
	g2.add_edge(2, 3, 7.77);
	g2.add_edge(3, 2, 5.55);
	g2.add_edge(3, 4, 6.66);
	cout << "Example with Matrix g2\n";
	cout << "----------------------\n";
	cout << endl;
	g2.display();
	cout << endl;
	g2.isSC() ? cout << "It is strongly connected !!\n" : cout << "It is not strongly connected :-(\n"; // Should be NO here !!
	cout << endl;
	g2.calcDegree();
	cout << endl;
	nb = g2.calcCycles();
	nb >0 ? cout << "It has " << nb << " cycles\n" : cout << "It has no cycle\n";
	cout << endl;
	// system("pause");


	cout << endl;
	cout << "Example of BFS with Matrix g1\n";
	cout << "------------------------------\n"; 
	m_visited = new bool[5];   //use 'new' to create array of size x//
							   // initialize visited
	for (i = 0; i < 5; i++)
		m_visited[i] = false;

	cout << endl;
	g1.display();
	cout << endl;
	g1.bfs(1, m_visited);
	delete(m_visited);
	cout << endl;
	// system("pause");


	cout << endl;
	cout << "Example of BFS with Matrix g3\n";
	cout << "------------------------------\n";
	AdjacencyMatrix g3(5); // n must be detected autimatically
	g3.add_edge(1, 3, 4.5);
	g3.add_edge(1, 5, 1.5);
	g3.add_edge(2, 5, 9.34);
	g3.add_edge(3, 4, 2.1);
	g3.add_edge(4, 1, 33.98);
	g3.add_edge(3, 5, 9.12);
	g3.add_edge(5, 3, 3.1415);
	g3.add_edge(5, 2, 111);
	m_visited = new bool[5];   //use 'new' to create array of size x//
							   // initialize visited
	for (int i = 0; i < 5; i++)
		m_visited[i] = false;

	cout << endl;
	g3.display();
	cout << endl;
	g3.bfs(1, m_visited);
	delete(m_visited);
	cout << endl;
	// system("pause");

	cout << endl;
	cout << "DAG of g1" << endl;
	g1.IsDAG();

	cout << endl;
	cout << "DAG of g2" << endl;
	g2.IsDAG();

	cout << endl;
	cout << "DAG of g3" << endl;
	g3.IsDAG();

	
	AdjacencyMatrix g4(12); // n must be detected autimatically
	g4.add_edge(3, 4, 1); // C to D
	g4.add_edge(8, 4, 1);
	g4.add_edge(8, 9, 1);
	g4.add_edge(10, 6, 1);
	g4.add_edge(9, 10, 1);
	g4.add_edge(4, 1, 1);
	g4.add_edge(4, 6, 1);
	g4.add_edge(1, 2, 1);
	g4.add_edge(2, 5, 1);
	g4.add_edge(4, 5, 1);
	g4.add_edge(6, 5, 1);
	g4.add_edge(6, 7, 1);
	g4.add_edge(6, 11, 1);
	g4.add_edge(7, 5, 1);
	g4.add_edge(7, 12, 1);
	g4.add_edge(11, 12, 1);

	cout << endl;
	cout << "DAG of g4" << endl;
	g4.IsDAG();

	
	AdjacencyMatrix g5(9);
	g5.add_edge(1, 2, 4);
	g5.add_edge(1, 8, 8);
	g5.add_edge(2, 8, 11);
	g5.add_edge(2, 3, 8);
	g5.add_edge(3, 9, 2);
	g5.add_edge(9, 7, 6);
	g5.add_edge(8, 9, 7);
	g5.add_edge(8, 7, 1);
	g5.add_edge(3, 4, 7);
	g5.add_edge(3, 6, 4);
	g5.add_edge(7, 6, 2);
	g5.add_edge(4, 6, 14);
	g5.add_edge(4, 5, 9);
	g5.add_edge(6, 5, 10);
	g5.AddBiDir();

	cout << endl;
	cout << "Dijkstra of g5" << endl;
	g5.dijkstra(1);

	
	AdjacencyMatrix g6(12);
	g6.add_edge(1, 2, 20);
	g6.add_edge(1, 4, 21);
	g6.add_edge(2, 4, 13);
	g6.add_edge(4, 6, 14);
	g6.add_edge(6, 5, 22);
	g6.add_edge(4, 5, 10);
	g6.add_edge(2, 3, 7);
	g6.add_edge(2, 5, 5);
	g6.add_edge(3, 5, 1);
	g6.add_edge(6, 7, 4);
	g6.add_edge(5, 7, 15);
	g6.add_edge(5, 8, 6);
	g6.add_edge(7, 8, 11);
	g6.add_edge(7, 9, 3);
	g6.add_edge(8, 9, 2);
	g6.add_edge(7, 10, 19);
	g6.add_edge(9, 10, 18);
	g6.add_edge(10, 11, 17);
	g6.add_edge(9, 11, 12);
	g6.add_edge(8, 11, 8);
	g6.add_edge(8, 12, 9);
	g6.add_edge(11, 12, 16);

	g6.AddBiDir();

	cout << endl;
	cout << "Dijkstra of g6" << endl;
	g6.dijkstra(11); 

	
	cout << endl;
	system("pause");
	return 0;
}
```
