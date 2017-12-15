/* This program allows to perform calculation on a city metro network
It includes an algorithm on c++ that allows to calculate the shortest and the quickest path
from one given metro station to another metro station(destination).

//
// Version 1.0 - 14/12/2017
// Author: Jean-Marc Uzé
// Status: done

Data structure:
[Vertices]			// The vertices includes all station names. If a station belongs to n lines of metro,
0000 Abbesses			// then the station name has n entries in this list
0001 Alexandre Dumas
0002 Alma Marceau
0003 Alésia
0004 Anatole France
0005 Anvers
0006 Argentine
0007 Arts et Métiers		// this station is repeated twice because because it belongs to two different lines
0008 Arts et Métiers
0009 Assemblée Nationale
0010 Aubervilliers-Pantin, Quatre Chemins
0011 Avenue Émile Zola
0012 Avron
0013 Barbès Rochechouart
0014 Barbès Rochechouart
...
0373 Église d'Auteuil
0374 Église de Pantin
0375 Étienne Marcel		// There are here 375 Vertices (stations)
[Edges] All edges are direct connections between two stations: <FromStation> <ToStation> <Time>
0 238 41
0 159 46
1 12 36				// It takes 36s from station Abbesses to station Avron
1 235 44
2 110 69
2 139 50
3 262 33
3 210 41
4 171 43
4 251 35
5 13 67
5 239 54
...
12 1 36			// It takes 36s from station Avron to station Abbesses
13 151 57
13 5 67
...
7 8 120.0		// It takes 120s (by foot) to change from one line to another in station Arts et Métiers
8 7 120.0
13 14 120.0
14 13 120.0
16 18 120.0
...
355 354 120.0
366 367 120.0
367 366 120.0	// There are here 367 Edges (stations)

Nb of Vertices: 375.
Nb of Edges: 367

There are 0,26% nb Edges compared to nb Vertices. This is Sparce Matrix.
We should then implement an Adjacency Matrix as a Spar

Dev Strategy:
- Define a class for vertex-edge pair and a class for the Network matrix, as well as a class to store the list of stations
- Create an array strings to store all names of the stations
- Create an array of linked-lists storing these pairs (Note a tutorial on https://www.codeproject.com/articles/24684/how-to-create-linked-list-using-c-c)
- Note: we will use two weights: the <distance> and the <time>
Example 1: the object corresponding to Avron(12)-Abesses(1) will store 1 as adjacent vertex, 1 as distance and 36 as time
Example 2: the object corresponding to Arts et Métiers(7)-Arts et Métiers(8) will store 8 as adjacent vertex, 0 as distance and 120 as time for train line change
- Read the data file and store the data as following:
1- First (ask for name and) read until we find [Vertices] and populate all names of station in the in the stations array, until we find [Edges]
2- Then create one object Pair per line and store the adjacency and the two attributes distance and time until we get an empty line or end of file
- We can then display a menu proposing 1) Calculate the shortest the path 1) Calculate the quickest the path 3) Print the station names 4) Exit
- Implement a Dijskra algorithm for the search, and launch it with time parameter, or distance depending on user choice
- Display the result
*/


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>
#include <cstdlib>
#include <locale.h>



using std::string;
using std::cin;

# define DISTANCE_SAME_STATION 0
# define DISTANCE_NEXT_STATION 1
# define MAX_CHAR_PER_LINE 1024

const int INF = 10000;	// infinity value used in Dijskra algorithm

/**************************************************************************************/
/**********************               Class Pair               ************************/
/**************************************************************************************/

// Define a class for vertex-edge pair
class Pair 
{
private:
	int m_edge_distance;		// Distance to next station, DISTANCE_NEXT_STATION if station to station,
					// and DISTANCE_SAME_STATION if changing train line
	int m_edge_time;		// Time to next station or time needed to change train line
	int m_adjacent_vertex;		// Next station index. Pair are direction sensitive, i.e. two objects to manage both directions
	Pair * m_next;			// next Pair object in the list

public:
	Pair(bool changeline, int edge_time, int adjacent_vertex);

	/*// Append a new Pair at the end of Pair list
	void AppendPair(Pair * headPair, int edge_time, int adjacent_vertex, bool changeline);*/
	
	// returns distance to next station
	int GetDistance() { return(m_edge_distance); };		

	// returns time to next station
	int GetTime() { return(m_edge_time); };					

	// returns Adjacency of the current Station
	int GetAdjacentVertex() { return(m_adjacent_vertex); };	

	// Get next Pair in the list of adjacencies
	Pair * GetNext() { return(m_next); };

	// Set next Pair in the list of adjacencies
	void SetNext(Pair* NextPair) { m_next = NextPair; };


	~Pair();
};


Pair::Pair(bool changeline, int edge_time, int adjacent_vertex)
{
	m_edge_time = edge_time;
	m_adjacent_vertex = adjacent_vertex;
	if (changeline == true)
	{
		m_edge_distance = DISTANCE_SAME_STATION;
	}
	else m_edge_distance = DISTANCE_NEXT_STATION;
	m_next = NULL;
}

Pair::~Pair()
{

}


/**************************************************************************************/
/********************             Class StationList               *********************/
/**************************************************************************************/

class StationList
{
private:
	std::string * m_name;		// array of all Stations and Adjacencies
	int m_size;				// Nb of stations in the array

public:
	StationList();		
	void SetArraySize(int size);			// set the size of the array m_name
	void AddStation(int index, char * name);	// write in the array a station with a given name at a given index
	std::string GetName(int index);			// returns the name of the station at given index
	int GetIndex(char * name);			// returns the index of the station with given name
	void Print(int n);				// print n first stations in the array with their index, all if n=0
	~StationList();
};

StationList::StationList()
{
	m_name = NULL;
	m_size = 0;
}

// set the size of the array m_name
void StationList::SetArraySize(int size)
{
	m_name = new std::string[size];
	m_size = size;
}

// write in the array a station with a given name at a given index
void StationList::AddStation(int index, char * name)
{
	m_name[index].assign(name);
}


// returns the name of the station at given index
std::string StationList::GetName(int index)
{
	return(m_name[index]);
}


// returns the index of the station with given name	- NOT IMPLEMENTED
int StationList::GetIndex(char * name)
{
	return(0);
}


// print n first stations in the array with their index, all if n=0
void StationList::Print(int n)
{
	int i;
	int nbprint;

	if (n == 0)
	{
		nbprint = this->m_size;
		printf("\nHere is the list of all stations (%d):\n", nbprint);
	}
	else
	{
		nbprint = n;
		printf("Here is the list of the first %d stations:\n", nbprint);
	}
	
	printf("Index\t\tStation Name");

	for (i = 0; i < nbprint; i++)
	{
		printf("%d\t\t%s\n", i, m_name[i].c_str());
	}
}


StationList::~StationList()
{
	// int i;
	// for (i=0; i<m_size; i++) delete(m_name[i]);
}



/**************************************************************************************/
/**********************             Class Matrix               ************************/
/**************************************************************************************/

class Matrix
{
private:
	Pair ** m_matrix;		// array of all stations' adjacencies
	StationList m_stations;		// array of all Stations with their index and name
	int m_size;			// Nb of stations in the array

public:
	// Create a Matrix from a file
	Matrix(char * station_filename);

	// returns a specific field from a line string at position num
	const char* GetField(char* line, int num, char * searchChar);	

	//     Remove given section from string. Negative len means remove
	//     everything up to the end
	int str_cut(char *str, int begin, int len);

	// search a position in the file where to start reading. Returns nb lines read	
	int FilePosSearch(FILE * file, char * keyword);		

	// print n first stations in the array with their index, all if n=0
	void Print(int n);		

	// print the Metro matrix - This is just for debugging/testing
	void PrintMatrix();

	// A utility function to find the vertex with minimum distance value, from
	// the set of vertices not yet included in shortest path tree
	int minDistance(int dist[], bool sptSet[]);

	// A utility function to print the constructed distance array
	void printDijkstraResults(int dist[], int n);

	// Funtion that implements Dijkstra's single source shortest path algorithm
	// src is the starting station, and weight is = 0 if distance is used, and 1 if time is used
	void dijkstra(int src, int dst, bool weight_type);

	// return nb stations in the network
	int GetSize() { return (m_size); };

	~Matrix();
};

// search a position in the file where to start reading. Returns nb lines read
// note: the file must be opened before calling the function
int Matrix::FilePosSearch(FILE * file, char * keyword)
{
	int i = 0;
	char line[MAX_CHAR_PER_LINE];

	// This loop reads all line and check if it corresponds exactly to the keyword
	// If yes then we found the section and we can return
	while (fgets(line, 1024, file))
	{
		i++;
		char* tmp = strdup(line);
		if (strcmp(keyword, tmp) == 0) break;
		free(tmp);
	}
	return(i); // returns nb lines read
}

// returns a specific field from a line string at position num
const char* Matrix::GetField(char* line, int num, char * searchChar)
{
	char buffer[1024];
	char *token = strtok(line, searchChar);
	int i = 0;

	while (token) {

		if (!--num) return token;
		token = strtok(NULL, searchChar);
	}

	return NULL;

}


//     Remove given section from string. Negative len means remove
//      everything up to the end.
//		This function is finally not used in this program. I keep it for records
int Matrix::str_cut(char *str, int begin, int len)
{
	int l = strlen(str);

	if (len < 0) len = l - begin;
	if (begin + len > l) len = l - begin;
	memmove(str + begin, str + begin + len, l - len + 1);

	return len;
}


Matrix::Matrix(char * station_filename)
{
	int nbLines;	// nb lines read in the last search. Allows us to count number of vertices/stations and define the array size
	int nbStations;	// temporary counters to store nbStations

	setlocale(LC_ALL, "French");	// allows to read french character (accents) in the file
	FILE* MetroFile = fopen(station_filename, "r");

	if (MetroFile != NULL)
	{
		
		// Step 1: count the number of stations
		nbLines = FilePosSearch(MetroFile, "[Vertices]\n");	// we found the start of the section. We can now start to count
		nbStations = FilePosSearch(MetroFile, "[Edges]\n") - 1;	// Count all stations in the file

		// Step 2: create the array of stations with the right size
		m_stations.SetArraySize(nbStations);

		// Step 3: search the section containing all station names with their index
		rewind(MetroFile);			// restart from top to find the beginning of the section
		nbLines = FilePosSearch(MetroFile, "[Vertices]\n");
		
		// Step 4: read all stations with their index and store them in the array, until we find the Edges section
		// THIS MUST BE REVIEWED AND FIXED AS IT DOESN'T WORK AS DESIGNED
		char line[MAX_CHAR_PER_LINE];
		int i = 0;		// number of lines read
		int imax = 0;	// max index found in the file. It should be equal to i but just in case the file is not consistent

		int index=0;
		char name[100];
		//while ((i<nbStations) && (fgets(line, 1024, MetroFile)))
		while ((i<nbStations) && (fscanf(MetroFile, "%d%[^\n]", &index, name))) // fscanf is actually great to read a file !!!
		{
			m_stations.AddStation(index, name);			// Add them to the array of stations
			if (index > imax) imax = index;
			i++;
			if (i > imax) imax = i;
		}

		// Step 5: Initialize the matrix array of Pairs with the right size
		m_matrix = new Pair*[nbStations];
		m_size = nbStations;			// store the number of lines in Matrix private data.
		for (i = 0; i < m_size; i++) m_matrix[i] = NULL;
	
		// Step 6: count the number of edges in the file
		nbLines = 0;
		while (fgets(line, 1024, MetroFile)) nbLines++;

		// Step 2: create a tmp array of edges with the right size
		Pair ** newEdges;
		newEdges = new Pair*[nbLines];

		// Step 5: store all edges in the Matrix

		rewind(MetroFile);			// restart from top to find the beginning of the section
		FilePosSearch(MetroFile, "[Edges]\n");	// find the edges section

		i = 0;
		while (fgets(line, 1024, MetroFile))
		{
			char* tmp = strdup(line);
			char* FromStation_c = strdup(GetField(tmp, 1, " \t"));	// read the departure station
			int FromStation = std::stoi(FromStation_c);		//converts the station index from char to int
			free(tmp);
			
			tmp = strdup(line);					// because tmp was destroyed by last getfield call
			char* ToStation_c = strdup(GetField(tmp, 2, " \t"));	// read the arrival station
			int ToStation = std::stoi(ToStation_c);			//converts the station index from char to int
			free(tmp); 
			
			tmp = strdup(line);					// because tmp was destroyed by last getfield call
			char* Time_c = strdup(GetField(tmp, 3, " \t"));		// read the travel time
			int time = std::stoi(Time_c);				//converts the travel time from char to int
			free(tmp);

			// We test if we change line based on time. We should not do it this way
			// We should compare the FromStation and check if equal, but we used this for now as a workable workaround
			bool changeline;
			if (time == 120)
				changeline = true;
			else
				changeline = false;

			newEdges[i]= new Pair(changeline, time, ToStation);		// create the new Pair
			
			// Append the current list of adjacencies (if exists) to the new Pair created
			if (m_matrix[FromStation] != NULL)
			{
				newEdges[i]->SetNext(this->m_matrix[FromStation]);
			}
			// Attach the updated list of adjacencies to the Network matrix
			m_matrix[FromStation] = newEdges[i];
			
			i++;

		}


		
		fclose(MetroFile);	// close the file

	}

}

// print n first stations in the array with their index, all if n=0
void Matrix::Print(int n)
{
	m_stations.Print(n);
}


// print the Metro matrix - This is just for debugging/testing
void Matrix::PrintMatrix()
{

	int i;
	Pair * tmp;

	for (i = 0; i < m_size; i++)
	{
		tmp = m_matrix[i];
		printf("PRINT THE NETWORK (FOR TESTING)\n");
		printf("\nStation %d:\n",i);
		while (tmp != NULL)
		{
			printf("To: %d\tTime: %d\tDistance: %d\n", tmp->GetAdjacentVertex(), tmp->GetTime(), tmp->GetDistance());
			tmp = tmp->GetNext();
		}
		printf("\n");
	}
}



// ----------------------------------------------------------------------------
//                Section on DIJKSTRA 
//
//		This is an adaptation of the code developped in previous class assesment 
//	
// ----------------------------------------------------------------------------

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int Matrix::minDistance(int dist[], bool sptSet[])
{
	// Initialize min value
	int min = INF;
	int min_index, v;

	for (v = 0; v < m_size; v++)
		if (sptSet[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}

// A utility function to print the constructed distance array to all destination from single source
void Matrix::printDijkstraResults(int dist[], int n)
{
	int i;
	printf("Station   -    Distance from Source\n");
	for (i = 0; i < n; i++)
		printf("%d                %d\n", i, dist[i]);
}

// Funtion that implements Dijkstra's single source shortest path algorithm
// src is the starting station, and weight is = 0 if distance is used, and 1 if time is used
void Matrix::dijkstra(int src, int dst, bool weight_type)
{

	// ON GOING WORK

	int i;
	int count;

	int *dist;				// The output array.  dist[i] will hold the shortest
	dist = new int[m_size];	 		// distance from src to i

	bool *sptSet;				// sptSet[i] will true if vertex i is included in shortest
	sptSet = new bool[m_size];		// path tree or shortest distance from src to i is finalized

	int *Parents;
	Parents = new int[m_size];		// Store Parent of node (i.e. the previous node in the path) 

	int *ParentsRev;
	ParentsRev = new int[m_size];		// Reverse order of Parents to print the best path

	// Initialize all distances as INFINITE and stpSet[] as false
	for (i = 0; i < m_size; i++)
		dist[i] = INF, sptSet[i] = false;

	// Distance of source vertex from itself is always 0
	dist[src] = 0;

	// Find shortest path for all vertices
	for (count = 0; count < m_size - 1; count++)
	{
		// Pick the minimum distance vertex from the set of vertices not
		// yet processed. u is always equal to src in first iteration.
		int u = minDistance(dist, sptSet);

		// Mark the picked vertex as processed
		sptSet[u] = true;

		int weight_value;
		int adj;
		Pair * tmp;

		// Update dist value of the adjacent vertices of the picked vertex.
		tmp = m_matrix[u]; 
		while (tmp != NULL)
			{	
				if (weight_type == 0)
					weight_value = tmp->GetDistance();		// weight is distance 
				else
					weight_value = tmp->GetTime();		// weight is time 
				adj = tmp->GetAdjacentVertex();
				// Update dist[adj] only if is not in sptSet, there is an edge from 
				// u to adj, and total weight of path from src to  adj through u is 
				// smaller than current value of dist[adj]
				if (!sptSet[adj] && dist[u] != INF
					&& dist[u] + weight_value < dist[adj])
				{
					dist[adj] = dist[u] + weight_value;
					Parents[adj] = u;
				}
					
				if (adj == dst) break;		// We found the destination and we can stop here.
				tmp = tmp->GetNext();
			}
		if (adj == dst) break;				// We found the destination and we can stop here.
	}
	
	// print the constructed distance array
	// printDijkstraResults(dist, m_size);


	i = dst;
	int nbParents = 0;
	
	// Create the reverse order of stations to store the complete path
	while (i != src)
	{
		
		ParentsRev[nbParents] = Parents[i];
		nbParents++;
		i = Parents[i];
	}

	i = 0;
	string str1;
	string str2;

	// Print now the stations in the right order to follow the path from source to destination
	int numStation = 0;
	for (i = nbParents - 1; i >= 0; i--)
	{

		printf("%d\t%s\n", numStation, this->m_stations.GetName(ParentsRev[i]).c_str());

		if (i > 0)	// Avoid duplicated hop counting
		{
			string str1 = this->m_stations.GetName(ParentsRev[i]).c_str();
			string str2 = this->m_stations.GetName(ParentsRev[i - 1]).c_str();
			if (str1.compare(str2) != 0) numStation++;
		}

	}

	delete(dist);
	delete(sptSet);
}





Matrix::~Matrix()
{

}



int main()

{

	char filename[250];
	FILE* MFile = fopen(filename, "r");

	while (MFile==NULL)
	{
		std::cout << "enter Metro file name: ";
		std::cin >> filename;
		std::cout << filename << '\n';
		MFile = fopen(filename, "r");
	}

	fclose(MFile);

	// For testing
	//Matrix Metro("metro_complet.txt");
	
	Matrix Metro(filename);
	
	// Show all stations and numbers
	Metro.Print(0);
	//system("pause");

	// For testing
	//Metro.PrintMatrix();

	

	bool ExitMenu = false;

	do
	{
		printf("\n");
		printf("*******************************************************************\n");
		printf("\t 1. Find the shortest Path \n");
		printf("\t 2. Find the quickest Path \n");
		printf("\t 0. Exit \n");
		printf("*******************************************************************\n");


		int nUserChoice;


		// We need to iterate as long as nUserChoice is not in [0;3]

		do
		{
			printf("Select your choice: ");
			scanf("%d", &nUserChoice);

		} while (nUserChoice < 0 || nUserChoice >5);


		switch (nUserChoice)
		{
		case 1:
		{
			int nbsrc;
			printf("Please enter Source station index: ");
			while ((scanf("%d", &nbsrc) != 1) || (nbsrc >= Metro.GetSize()) || (nbsrc < 0))
			{
				printf("Please enter Source station index: ");
				while (getchar() != '\n');
			}

			int nbdst;
			printf("Please enter Destination station index: ");
			while ((scanf("%d", &nbdst) != 1) || (nbdst >= Metro.GetSize()) || (nbdst < 0))
			{
				printf("Please enter Destination station index: ");
				while (getchar() != '\n');
			}
			
			printf("\nHere is the shortest Path with the list of stations\n");
			printf("Note: the stations are duplicated when there is a line change, but they are counted as a single hop\n");

			Metro.dijkstra(nbsrc, nbdst, 0);
			system("pause");

		}
		break;

		case 2:
		{
			int nbsrc;
			printf("Please enter Source station index: ");
			while ((scanf("%d", &nbsrc) != 1) || (nbsrc >= Metro.GetSize()) || (nbsrc < 0))
			{
				printf("Please enter Source station index: ");
				while (getchar() != '\n');
			}

			int nbdst;
			printf("Please enter Destination station index: ");
			while ((scanf("%d", &nbdst) != 1) || (nbdst >= Metro.GetSize()) || (nbdst < 0))
			{
				printf("Please enter Destination station index: ");
				while (getchar() != '\n');
			}

			printf("\nHere is the quickest Path with the list of stations\n");
			printf("Note: the stations are duplicated when there is a line change, but they are counted as a single hop\n");

			Metro.dijkstra(nbsrc, nbdst, 1);
			system("pause");
		}
		break;

		case 0:
		{
			ExitMenu = true;
		}

		default: // default can be used for the same as case 0 : exit
				 //or default can be used to "do nothing"
			break;

		}

	} while (ExitMenu == false);

	
	return(0);
}



