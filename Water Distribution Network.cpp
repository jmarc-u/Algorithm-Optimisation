// WARNING : Data Preparation from source file - to be done manually before running the program
// - Extract all Junctions/Tanks/Reservoirs and put them in JUNCTIONS_FILE
// - Extraxt all Tanks/Reservoirs and put them in TANKSRESERVOIRS_FILE
// - Extract all Pipes/Pumps and put them in PIPES_FILE
// - Extract all Valves and put them in VALVES_FILE
//
// Version 1.0 - 12/12/2017
// Author: Jean-Marc Uzé
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>


# define JUNCTIONS_FILE "Pd2_JUNCTIONS.txt"
# define PIPES_FILE "Pd2_PIPES.txt"
# define TANKSRESERVOIRS_FILE "Pd2_TANKSRESERVOIRS.txt"
# define VALVES_FILE "Pd2_VALVES.txt"
# define IS_TANKRESERVOIR -2
# define IS_PIPE 1
# define IS_VALVE -3
# define IS_JUNCTION 0



// Pipe represents either a water pipe or a valve. A special case of Pipe is a Junction.
class Pipe
{
private:
	int m_adj;			// the junction index it connects to (either if a Pipe or a Valve)
						// =0 if it is a Junction and not a pipe
	char* m_ID = NULL;	// the ID of the Pipe or the Valve, or the ID of the Junction if no adjacency
	int m_type;			// Type of the pipe (=IS_JUNCTION if a Junction, =IS_PIPE if a Pipe, =IS_VALVE if a Valve
	Pipe* m_next;

public:
	Pipe() { m_ID = NULL; m_next = NULL; }; // initialize with zero element
	Pipe(int adj, char* ID, int type);
	void SetNext(Pipe* aNext) { m_next = aNext; };
	void SetAdj(int adj) { m_adj = adj; };
	void SetType(int type) { m_type = type; };
	void SetID(char *ID) { m_ID = new char[strlen(ID) + 1]; strcpy(m_ID, ID); };
	Pipe* GetNext() { return(m_next); };
	int GetAdj() { return m_adj; };
	char* GetID() { return m_ID; };
	int GetType() { return m_type; };
	~Pipe();
};


// Constructor of Pipe
Pipe::Pipe(int adj, char* ID, int type) // with adjacency Junction, the name of the pipe/valve, and the type if pipe *or* valve
{
	m_adj = adj;
	m_type = type;
	m_ID = new char[strlen(ID) + 1];
	strcpy(m_ID, ID);
	m_next = NULL;
}


// Destructor of Pipe
Pipe::~Pipe()
{
	if (m_ID != NULL)
		delete m_ID;

	//if (m_next != NULL)
		//delete m_next;
}


// this is a line in my Network array, i.e. collecting all adjacencies (pipes or valves) 
// of a given Junction.
class Junction_Adj
{
private:
	Pipe *m_head;

public:
	Junction_Adj() { m_head = NULL; }; // initialize with zero element
	Junction_Adj(Pipe * pipe); // initialize with the first element which should be a Junction, not a Pipe.
	void Append(Pipe * pipe);
	Pipe * GetHead() { return m_head; };
	~Junction_Adj();
};


// Constructor of Junction_Adj
Junction_Adj::Junction_Adj(Pipe* pipe)
{ 
	m_head = pipe; 
}

// Add one Pipe object to the list of adjacencies of a given Junction
void Junction_Adj::Append(Pipe * pipe)
{

	// Create a temp pointer
	Pipe *tmp = m_head;

	if (tmp != NULL) {
		// Some Pipes already present in the list
		// Parse to end of list
		while (tmp->GetNext() != NULL) {
			tmp = tmp->GetNext();
		}

		// Point the last Pipe to the new Pipe
		tmp->SetNext(pipe);
	}
	else {
		// First node in the list
		m_head = pipe;
	}

}

// Destructor of Junction_Adj
Junction_Adj::~Junction_Adj()
{
}


class Network
{
private:
	Junction_Adj *m_graph;
	int m_Nb_Junctions;

public:
	Network(int N); // initialize with a size of N junctions - Use for tests only when no data file available
	Network(); // Normal constructor
	void AddJunctionAdj(Junction_Adj * J_adj, int I); // Add row number I in the Graph, i.e. one Junction with all Adjacencies
	const char* getfield(char* line, int num); // returns a specific field from a line string at position num
	int ReadFile(char * J_filepath, char * P_filepath, char * V_filepath, char * TR_filepath); // read all Junctions/Pipe/Valves from respective files and tags the Tanks/Reservoirs
	void Print(int N); // Print the N lines in the Network
	std::list<std::string> BFSValve(int Junction);
	int GetNbJunctions() { return m_Nb_Junctions; };
	int SearchJunctionIndex(char * ID); // return the index used by a given Junction in the Network array
	int Network::SearchPipeIndex(char * ID); // return the index of one Junction of the pipe to repair in order to start the search from
	~Network();
};

// constructor used for the test of the program, filling the network manually as we have no data file
Network::Network(int N)
{
	// We will not use more than N Junctions for the manual test
	m_graph = new Junction_Adj[N];
	m_Nb_Junctions = N;
}

// Real constructor reading the Network from files
 Network::Network()
{
	 // Count the number of Junctions by counting lines in the file
	 unsigned int number_of_lines = 0;
	 FILE *infile = fopen(JUNCTIONS_FILE, "r"); // The file Junctions contains all Junctions including Tanks and Reservoirs
												//	FILE *infile = fopen("JUNCTIONS-ALL.txt", "r");
	 int ch;

	 while (EOF != (ch = getc(infile)))
		 if ('\n' == ch)
			 ++number_of_lines;

	 fclose(infile);

	 m_graph = new Junction_Adj[number_of_lines];
	 m_Nb_Junctions = number_of_lines;

	 // Fill the Network from files
	 // The file JUNCTIONS_FILE contains all Junctions including Tanks and Reservoirs
	 // However we use another file TANKSRESERVOIRS_FILE to store the Tanks and Reservoirs in order to set their Junction type as IS_TANKRESERVOIR and thus identify them in the BFS
	 // The file Pipes also includes all Pumps as defined the same way, i.e. we consider a Pump can not be closed like a Valve.
	 ReadFile(JUNCTIONS_FILE, PIPES_FILE, VALVES_FILE, TANKSRESERVOIRS_FILE);

}

// Add row number I in the Graph, i.e. one Junction with all Adjacencies
void Network::AddJunctionAdj(Junction_Adj * J_adj, int I)
{
	m_graph[I] = *J_adj;
}


// Print on screen the content of the stored network, one section per Junction + its adjacencies
/* One line example:

Junction 0 named 14N36b1 is connected to:
- Junction 5 named 14N182d via Pipe 14Tbe7
- Junction 8 named xxxxxxx via Valve yyyyy

*/
void Network::Print(int N)	// N is the number of Junctions to print (the first N) with its Adjacencies
{
	int i;

	for (i = 0; i < N; i++)
	{
		Pipe * Ptmp = m_graph[i].GetHead();
		int Adjtmp = Ptmp->GetAdj();
		char * IDtmp = Ptmp->GetID();
		int Typetmp = Ptmp->GetType();
		if ((Ptmp != NULL) && (Typetmp ==IS_JUNCTION))
		{
			printf("\nJunction %d named %s is connected to:\n", Adjtmp, IDtmp);
			Ptmp = Ptmp->GetNext();
		}
		while (Ptmp != NULL)
		{
			Adjtmp = Ptmp->GetAdj();			
			IDtmp = Ptmp->GetID();
			Typetmp = Ptmp->GetType();
			//Pipe * Next_head = m_graph[Adjtmp].GetHead();
			printf("- Junction %d named %s via ", Adjtmp, m_graph[Adjtmp].GetHead()->GetID());
			if (Typetmp == IS_PIPE) { printf(" Pipe ");} else if (Typetmp == IS_VALVE) printf(" Valve ");
			printf("%s\n", IDtmp);
			Ptmp = Ptmp->GetNext();
		}
	}
}



// An iterative function to find Valves starting from Junction, using BFS algorithm
// Note: as we remove a pipe, we can start the search from any of its two junction, to all directions
std::list<std::string> Network::BFSValve(int Junction)
{
	std::list<int> queue; // queue for the BFS management
	std::list<std::string> valves; // queue to store found valves
	queue.push_back(Junction); // Add Junction A to the queue
	//unsigned count = 0;
	int nb;
	//int i;
	int nb_junctions = this->GetNbJunctions();
	bool * visited;
	visited = new bool[nb_junctions];

	// variables to manage the Junction adjacencies (line in the Network array)
	int Adjtmp; // = Ptmp->GetAdj();
	char * IDtmp; // = Ptmp->GetID();
	int Typetmp;  // = Ptmp->GetType();

	// initialize visited
	for (int i = 0; i < nb_junctions; i++)
		visited[i] = false;

	while (!queue.empty())
	{
		nb = queue.front(); // pick the first in the queue
		queue.pop_front();	// and remove it

		if (!visited[nb])
		{
			visited[nb] = true;
		}

		Pipe * Ptmp = m_graph[nb].GetHead();
		Typetmp = Ptmp->GetType();
		IDtmp = Ptmp->GetID();

		if (Typetmp == IS_TANKRESERVOIR)
		{
			char * tmp_ID = new char[200]; // a local trick to store a message error in the table
			tmp_ID[0] = '\0';
			strcat(tmp_ID, "WARNING: there is a connected Tank or Reservoir ID ");
			strcat(tmp_ID, IDtmp);
			strcat(tmp_ID, ". You can not isolate the pipe !");
			valves.push_back(tmp_ID);
			delete(tmp_ID);
		}
			

		Ptmp = Ptmp->GetNext();

		while (Ptmp != NULL)
		{
			Adjtmp = Ptmp->GetAdj();
			IDtmp = Ptmp->GetID();
			Typetmp = Ptmp->GetType();

			if ((Typetmp == IS_PIPE) && (!visited[Adjtmp]))
			{
				queue.push_back(Adjtmp);
				visited[Adjtmp] = true;
			}

			if (Typetmp == IS_VALVE) // it is Valve to next Junction
			{
				valves.push_back(IDtmp);
			}
			Ptmp = Ptmp->GetNext();
		}n
	}
	return(valves);
}


// returns a specific field from a line string at position num
const char* Network::getfield(char* line, int num)
{
	char buffer[1024];
		char *token = strtok(line, " \t");
		int i = 0;

		while (token) {

			if (!--num) return token;
			token = strtok(NULL, " \t");
		}

		return NULL;

}


// return the index used by a given Junction in the Network array
int Network::SearchJunctionIndex(char * ID)
{
	int i;
	Pipe * ipipe;
	char * iID;

	for (i = 0; i < m_Nb_Junctions; i++)
	{
		ipipe = m_graph[i].GetHead();
		iID = ipipe->GetID();
		if (strcmp(iID, ID) == 0) return i;
	}

	return -1;
}


// return the index of one Junction part of a given Pipe in the Network array
// This will be the starting point to search for all valvles to close 
int Network::SearchPipeIndex(char * ID)
{
	int i;
	Pipe * ipipe;
	char * iID;

	for (i = 0; i < m_Nb_Junctions; i++)
	{
		ipipe = m_graph[i].GetHead();

		if (ipipe != NULL) // The first one is just the Junction, wo we are looking for the next ones which are pipes and valves
		{
			while (ipipe->GetNext() != NULL)
			{
				ipipe = ipipe->GetNext();
				iID = ipipe->GetID();
				if (strcmp(iID, ID) == 0)
				{
					if (ipipe->GetType() == IS_PIPE) return i; // This is a Pipe
					if (ipipe->GetType() == IS_VALVE) return IS_VALVE; // this is Valve and not a Pipe
				}
			}
		}
	}

	return -1;
}

// read all Junctions/Pipe/Valves from respective files
int Network::ReadFile(char * J_filepath, char * P_filepath, char * V_filepath, char * TR_filepath)
{
	Pipe * J_All = new Pipe[m_Nb_Junctions];
	Junction_Adj * JAdj_All = new Junction_Adj[m_Nb_Junctions];


	FILE* J_file = fopen(J_filepath, "r");
	FILE* P_file = fopen(P_filepath, "r");
	FILE* V_file = fopen(V_filepath, "r");
	FILE* TR_file = fopen(TR_filepath, "r");

	if ((J_file == NULL) || (P_file == NULL) || (V_file == NULL) || (TR_file == NULL)) return -1;
	char line[1024];

	// READ all Junctions and store them in the Network array (as the first element of each list/line)
	// Note: it also includes Tanks and Reservoirs which are special Junctions. We will tag them (SetType) as such later.

	int i = 0;
	while (fgets(line, 1024, J_file))
	{
		char* tmp = strdup(line);
		char* ID = strdup(getfield(tmp, 1));
		J_All[i].SetAdj(i); // Create Junction as a Pipe with Adjacency to himself
		J_All[i].SetID(ID);
		J_All[i].SetType(IS_JUNCTION);
		J_All[i].SetNext(NULL); // We will later chain all adjacencies of this Junction
		JAdj_All[i].Append(&J_All[i]); // Create a Junction adjacency list with the first Junction
		this->AddJunctionAdj(&JAdj_All[i], i); // Populate the Network with this new created Adjacency list containin the first Junction
		free(tmp);
		free(ID);
		i++;
	}

	fclose(J_file);

	//
	// READ all Pipes and store them in the Network array to the Junction they are attached to
	//
	// Count the number of Pipes by counting lines in the file
	unsigned int number_of_lines = 0;
	int ch;

	while (EOF != (ch = getc(P_file)))
		if ('\n' == ch)
			++number_of_lines;
	rewind(P_file); // return to beginning of the file

	// create twice as many Pipes as they are in the file
	// Because we store the pipes in both directions for the search
	Pipe * P_All = new Pipe[number_of_lines*2]; 

	i = 0;
	while (fgets(line, 1024, P_file))
	{
		char* tmp = strdup(line);
		char* ID = strdup(getfield(tmp, 1));
		tmp = strdup(line); // because tmp was destroyed by last getfield call
		char* J1 = strdup(getfield(tmp, 2));
		tmp = strdup(line);  // because tmp was destroyed by last getfield call
		char* J2 = strdup(getfield(tmp, 3));	
		int J1_ind = this->SearchJunctionIndex(J1);
		int J2_ind = this->SearchJunctionIndex(J2);
		if ((J1_ind != -1) && (J2_ind != -1))
		{
			// First pipe
			P_All[i].SetAdj(J2_ind);
			P_All[i].SetID(ID);
			P_All[i].SetType(IS_PIPE);
			P_All[i].SetNext(NULL);
			JAdj_All[J1_ind].Append(&P_All[i]); // Add an adjacency with the corresponding J1 Junction

			// Second pipe (its symetric)
			P_All[i + number_of_lines].SetAdj(J1_ind);
			P_All[i + number_of_lines].SetID(ID);
			P_All[i + number_of_lines].SetType(IS_PIPE);
			P_All[i + number_of_lines].SetNext(NULL);
			JAdj_All[J2_ind].Append(&P_All[i + number_of_lines]); // Add an adjacency with the corresponding J1 Junction
		}
		
		free(tmp);
		free(ID);
		free(J1);
		free(J2);

		i++; 
	}

	fclose(P_file);

	//
	// READ all Valves and store them in the Network array to the Junction they are attached to
	//
	// Count the number of Valves by counting lines in the file
	number_of_lines = 0;

	while (EOF != (ch = getc(V_file)))
		if ('\n' == ch)
			++number_of_lines;
	rewind(V_file); // return to beginning of the file

	// create twice as many Valves as they are in the file
	// Because we store the valves in both directions for the search
	Pipe * V_All = new Pipe[number_of_lines * 2];

	i = 0;
	while (fgets(line, 1024, V_file))
	{
		char* tmp = strdup(line);
		char* ID = strdup(getfield(tmp, 1));
		tmp = strdup(line); // because tmp was destroyed by last getfield call
		char* J1 = strdup(getfield(tmp, 2));
		tmp = strdup(line);  // because tmp was destroyed by last getfield call
		char* J2 = strdup(getfield(tmp, 3));
		int J1_ind = this->SearchJunctionIndex(J1);
		int J2_ind = this->SearchJunctionIndex(J2);
		if ((J1_ind != -1) && (J2_ind != -1))
		{
			// First valve
			V_All[i].SetAdj(J2_ind);
			V_All[i].SetID(ID);
			V_All[i].SetType(IS_VALVE);
			V_All[i].SetNext(NULL);
			JAdj_All[J1_ind].Append(&V_All[i]); // Add an adjacency with the corresponding J1 Junction

			// Second valve (its symetric one)
			V_All[i + number_of_lines].SetAdj(J1_ind);
			V_All[i + number_of_lines].SetID(ID);
			V_All[i + number_of_lines].SetType(IS_VALVE);
			V_All[i + number_of_lines].SetNext(NULL);
			JAdj_All[J2_ind].Append(&V_All[i + number_of_lines]); // Add an adjacency with the corresponding J1 Junction
		}

		free(tmp);
		free(ID);
		free(J1);
		free(J2);

		i++;
	}

	fclose(V_file);


	//
	// READ all Tanks and Reservoirs and tag them in the Network array first Element (as these are Junctions)
	// Each Tank/Reservoir is modelized as a junctions (node).
	// We will tag both junctions with type = IS_TANKRESERVOIR
	// so that if we find any of them in the BFS, we will consider a water link that is impossible to avoid even by closing valves.
	//

	
	while (fgets(line, 1024, TR_file))
	{
		char* tmp = strdup(line);
		char* J1 = strdup(getfield(tmp, 1));
		int J1_ind = this->SearchJunctionIndex(J1);

		if (J1_ind != -1)
		{
			m_graph[J1_ind].GetHead()->SetType(IS_TANKRESERVOIR); // change the Type setting of this Junction to Tank/Reservoir
		}

		free(tmp);
		free(J1);
	}

	fclose(TR_file);

	return(0);
}

Network::~Network()
{
}


//--------------------------------------------------------------------------------------------------
//------------------------------     MAIN SECTION      ---------------------------------------------
//--------------------------------------------------------------------------------------------------


int main()
{


/* THIS SECTION WAS USED FOR TESTS - TO BE USED ONLY IF WE HAVE NO DATA FILE AVAILABLE:
---------------------------------------------------------------------------------------
	int adj;
	char * ID;
	int i;
	


	// Test Junctions
	Pipe P0_0(0, "P0", 0);
	Pipe P1_1(1, "P1", 0);
	Pipe P2_2(2, "P2", 0); 
	Pipe P3_3(3, "P3", 0);
	Pipe P4_4(4, "P4", 0);
	Pipe P5_5(5, "P5", 0);
	Pipe P6_6(6, "P6", 0);
	Pipe P7_7(7, "P7", 0);
	Pipe P8_8(8, "P8", 0);
	Pipe P9_9(9, "P9", 0);
	Pipe P10_10(10, "P10", 0);
	Pipe P11_11(11, "P11", 0);
	Pipe P12_12(12, "P12", 0);
	Pipe P13_13(13, "P13", 0);
	Pipe P14_14(14, "P14", 0);
	Pipe P15_15(15, "P15", 0);
	Pipe P16_16(16, "P16", 0);
	Pipe P17_17(17, "P17", 0);
	Pipe P18_18(18, "P18", 0);
	Pipe P19_19(19, "P19", 0);

	// Test Pipes
	Pipe P0_4(4, "P0_4", 1);
	Pipe P4_0(0, "P4_0", 1);
	Pipe P4_10(10, "P4_10", 1);
	Pipe P10_4(4, "P10_4", 1);
	Pipe P1_2(2, "P1_2", 1);
	Pipe P2_1(1, "P2_1", 1);
	Pipe P2_5(5, "P2_5", 1);
	Pipe P5_2(2, "P5_2", 1);
	Pipe P2_11(11, "P2_11", 1);
	Pipe P11_2(2, "P11_2", 1);
	Pipe P4_5(5, "P4_5", 1);
	Pipe P5_4(4, "P5_4", 1);
	Pipe P4_14(14, "P4_14", 1);
	Pipe P14_4(4, "P14_4", 1);
	Pipe P15_7(7, "P15_7", 1);
	Pipe P7_15(15, "P7_15", 1);
	Pipe P7_8(8, "P7_8", 1);
	Pipe P8_7(7, "P8_7", 1);
	Pipe P5_16(16, "P5_16", 1);
	Pipe P16_5(5, "P16_5", 1);
	Pipe P17_8(8, "P17_8", 1);
	Pipe P8_17(17, "P8_17", 1);
	Pipe P5_12(12, "P5_12", 1);
	Pipe P12_5(5, "P12_5", 1);
	Pipe P13_3(3, "P13_3", 1);
	Pipe P3_13(13, "P3_13", 1);
	Pipe P3_6(6, "P3_6", 1);
	Pipe P6_3(3, "P6_3", 1);
	Pipe P6_5(5, "P6_5", 1);
	Pipe P5_6(6, "P5_6", 1);
	Pipe P8_18(18, "P8_18", 1);
	Pipe P18_8(8, "P18_8", 1);
	Pipe P19_9(9, "P19_9", 1);
	Pipe P9_19(19, "P9_19", 1);

	//Test Valves
	Pipe P10_11(11, "P10_11", -1);
	Pipe P11_10(10, "P11_10", -1);
	Pipe P14_15(15, "P14_15", -1);
	Pipe P15_14(14, "P15_14", -1);
	Pipe P12_13(13, "P12_13", -1);
	Pipe P13_12(12, "P13_12", -1);
	Pipe P16_17(17, "P16_17", -1);
	Pipe P17_16(16, "P17_16", -1);
	Pipe P18_19(19, "P18_19", -1);
	Pipe P19_18(18, "P19_18", -1);

	//Complete Junction with all Adjacencies
	Junction_Adj A0(&P0_0);
	A0.Append(&P0_4);
	Junction_Adj A1(&P1_1);
	A1.Append(&P1_2);
	Junction_Adj A2(&P2_2);
	A2.Append(&P2_11);
	A2.Append(&P2_1);
	A2.Append(&P2_5);
	Junction_Adj A3(&P3_3);
	A3.Append(&P3_13);
	A3.Append(&P3_6);
	Junction_Adj A4(&P4_4);
	A4.Append(&P4_0);
	A4.Append(&P4_10);
	A4.Append(&P4_14);
	A4.Append(&P4_5);
	Junction_Adj A5(&P5_5);
	A5.Append(&P5_2);
	A5.Append(&P5_4);
	A5.Append(&P5_16);
	A5.Append(&P5_12);
	A5.Append(&P5_6);
	Junction_Adj A6(&P6_6);
	A6.Append(&P6_3);
	A6.Append(&P6_5);
	Junction_Adj A7(&P7_7);
	A7.Append(&P7_15);
	A7.Append(&P7_8);
	Junction_Adj A8(&P8_8);
	A8.Append(&P8_17);
	A8.Append(&P8_7);
	A8.Append(&P8_18);
	Junction_Adj A9(&P9_9);
	A9.Append(&P9_19);
	Junction_Adj A10(&P10_10);
	A10.Append(&P10_4);
	A10.Append(&P10_11);
	Junction_Adj A11(&P11_11);
	A11.Append(&P11_10);
	A11.Append(&P11_2);
	Junction_Adj A12(&P12_12);
	A12.Append(&P12_5);
	A12.Append(&P12_13);
	Junction_Adj A13(&P13_13);
	A13.Append(&P13_12);
	A13.Append(&P13_3);
	Junction_Adj A14(&P14_14);
	A14.Append(&P14_4);
	A14.Append(&P14_15);
	Junction_Adj A15(&P15_15);
	A15.Append(&P15_14);
	A15.Append(&P15_7);
	Junction_Adj A16(&P16_16);
	A16.Append(&P16_5);
	A16.Append(&P16_17);
	Junction_Adj A17(&P17_17);
	A17.Append(&P17_16);
	A17.Append(&P17_8);
	Junction_Adj A18(&P18_18);
	A18.Append(&P18_8);
	A18.Append(&P18_19);
	Junction_Adj A19(&P19_19);
	A19.Append(&P19_18);
	A19.Append(&P19_9);

	// Build the Network
	Network My_network(20);
	My_network.AddJunctionAdj(&A0,0);
	My_network.AddJunctionAdj(&A1, 1);
	My_network.AddJunctionAdj(&A2, 2);
	My_network.AddJunctionAdj(&A3, 3);
	My_network.AddJunctionAdj(&A4, 4);
	My_network.AddJunctionAdj(&A5, 5);
	My_network.AddJunctionAdj(&A6, 6);
	My_network.AddJunctionAdj(&A7, 7);
	My_network.AddJunctionAdj(&A8, 8);
	My_network.AddJunctionAdj(&A9, 9);
	My_network.AddJunctionAdj(&A10, 10);
	My_network.AddJunctionAdj(&A11, 11);
	My_network.AddJunctionAdj(&A12, 12);
	My_network.AddJunctionAdj(&A13, 13);
	My_network.AddJunctionAdj(&A14, 14);
	My_network.AddJunctionAdj(&A15, 15);
	My_network.AddJunctionAdj(&A16, 16);
	My_network.AddJunctionAdj(&A17, 17);
	My_network.AddJunctionAdj(&A18, 18);
	My_network.AddJunctionAdj(&A19, 19);

  
	// My_network.Print(20);

	//adj = P0.GetAdj();
	//ID = P0.GetID();

	//printf("Adj: %d - ID: %s\n", adj, ID);

	// Network My_network(20);

	//Junction_Adj J1()

	// system("dir");
	// system("pause");


	My_network.BFSValve(0);
	system("pause");

	My_network.BFSValve(1);
	system("pause");

	My_network.BFSValve(2);
	system("pause");

	My_network.BFSValve(3);
	system("pause");

	My_network.BFSValve(4);
	system("pause");

	My_network.BFSValve(5);
	system("pause");

	My_network.BFSValve(16);
	system("pause");



	// system("dir");

	End of TEST SECTION
	------------------ - */


	printf("Welcome to the best in class Water Network Advisor, empowered by JMU Copyright 2017\n\n");
	printf("File loading and initialization of the Network...\n\n");
	
	// Create the Network
	Network WaterNetwork;
	

	printf("The first 50 Junctions with their adjacencies are:\n");
	printf("--------------------------------------------------\n\n");
	WaterNetwork.Print(50);


	char RepairPipe_ID[25];
	int RepairPipe_ind;
	std::list<std::string> list_valves;
	bool exit = false;
	
	do 
	{
		printf("\nEnter the ID of the Pipe you want to repair (0 to exit): ");
		scanf("%24s", RepairPipe_ID);
		printf("\n\n");

		if ((strcmp(RepairPipe_ID, "0")) != 0)
		{
			RepairPipe_ind = WaterNetwork.SearchPipeIndex(RepairPipe_ID);


			if ((RepairPipe_ind != -1) && (RepairPipe_ind != IS_VALVE))
			{
				list_valves = WaterNetwork.BFSValve(RepairPipe_ind); // Get all Valves connected, using BFS algorithm

				//auto iter = list_valves.begin();
				std::list<std::string>::iterator iter1 = list_valves.begin();
				std::list<std::string>::iterator iter2 = list_valves.begin();

				int exists = false;

				printf("The list of Valves to close are as following:\n");
				printf("---------------------------------------------\n");
				//for each (std::string s in list_valves)
				while (iter1 != list_valves.end())
				{
					iter2 = iter1; // to compare starting with the next one in order to detect duplicates
					++iter2;
					while (iter2 != list_valves.end())
					{
						if (strcmp(iter2->c_str(), iter1->c_str()) == 0) exists = true; // we found a duplicate
						++iter2;
					} 

					//printf("%s\n", s.c_str()); //c_str() return the string as a string...
					if (exists == false) std::cout << *iter1 << std::endl; // print it if unique in the rest of the list
					//std::cout << *iter1 << std::endl;
					exists = false; 

					++iter1;
				}
				

			}
			else
			{
				if (RepairPipe_ind == IS_VALVE)
				{
					printf("Sorry but this is Valve and not a Pipe. Retry ...\n");
				} else
				{
					printf("Sorry but this Pipe ID doesn't exist in the Network. Try again.\n");
				}
			}

		}
		else exit = true;
		
	} while (exit == false);
	

return 0;
}
