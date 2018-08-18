#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <math.h>
#include <list>

using namespace std;
//to run using the gcc compiler, type in g++ <filename> -o <name of executable>

struct edgeData{
	int weight;
	int start;
	int end;
};

struct graphEdge{
	int weight;
	int endNodeLabel;
};

struct graphNode{
	float rankCurr;
	float rankNext;
	int firstEdge;
	int numEdgesOut;
	int numEdgesIn;
};

bool edgeCompareTranspose(edgeData firstEdge, edgeData secondEdge){
	if(firstEdge.end < secondEdge.end){
		return true;
	}else{
		return false;
	}
}

bool edgeCompare(edgeData firstEdge, edgeData secondEdge){
	if(firstEdge.start < secondEdge.start){
		return true;
	}else{
		return false;
	}
}

void pushStylePR(graphNode* sentNodes, graphEdge* sentEdges, int numNodes, int numEdges){
	float dampFact = 0.85;
	float totSum;
	float convergenceFactor = pow(10, -4);
	bool converged = 0;

	//initialize all ranks to some value
	for(int i = 0; i < numNodes; i++){
		(sentNodes+i)->rankCurr = 1/numNodes;
		(sentNodes+i)->rankNext = 0;
	}

	//iterate some number of times
	while(!converged){
		totSum = 0;
		//iterate through all of the edges
		for(int j = 0; j < numNodes; j++){
			//if the node has incoming edges
			if((sentNodes+j)->numEdgesOut >= 1){
				int startEdge = (sentNodes+j)->firstEdge;
				int finalEdge = numEdges-1;
				int currentEdgeCheck = j+1;
				bool found = false;

				//find the starting and final edges for a node
				while((currentEdgeCheck < numNodes)&&(!found)){
					if((sentNodes+currentEdgeCheck)->numEdgesOut > 0){
						found = true;
						finalEdge = (sentNodes+currentEdgeCheck)->firstEdge - 1;
					}
					currentEdgeCheck++;
				}
				//change the rank for the node
				for(int k = startEdge; k <= finalEdge; k++){
					((sentNodes+((sentEdges+k)->endNodeLabel)-1)->rankNext) += ((sentNodes+j)->rankCurr)/(sentNodes+j)->numEdgesOut;
				}
			}
		}

		//check for convergence, as well as sets the new value for the current rank
		//and resets the value for nextRank
		converged = 1;
		for(int k = 0; k < numNodes; k++){
			float nextRank = ((1-dampFact)/(numNodes))+dampFact*((sentNodes+k)->rankNext);

			float convergenceCompare = (sentNodes+k)->rankCurr - nextRank;
			if((convergenceCompare >= convergenceFactor)||(convergenceCompare <= (-1)*convergenceFactor)){
				converged = 0;
			}
			(sentNodes+k)->rankCurr = nextRank;
			(sentNodes+k)->rankNext = 0;
			totSum += (sentNodes+k)->rankCurr;
		}
	}

	//print out the final rank for all of the pages
	cout << "The end Status for the nodes " << endl;
	for(int k = 0; k < numNodes; k++){
		(sentNodes+k)->rankCurr = (sentNodes+k)->rankCurr/totSum;
		cout << k+1 << " node rank = " << (sentNodes+k)->rankCurr << endl;
	}
}



void pullStylePR(graphNode* sentNodes, graphEdge* sentEdges,  int numNodes, int numEdges){
	float dampFact = 0.85;
	float totSum;
	float convergenceFactor = pow(10, -4);
	bool converged = 0;

	//initialize all ranks to some value
	for(int i = 0; i < numNodes; i++){
		(sentNodes+i)->rankCurr = 1/numNodes;
		(sentNodes+i)->rankNext = 0;
	}

	//iterate some number of times
	while(!converged){
		totSum = 0;
		//iterate through all of the edges
		for(int j = 0; j < numNodes; j++){
			//if the node has incoming edges
			if((sentNodes+j)->numEdgesOut >= 1){
				int startEdge = (sentNodes+j)->firstEdge;
				int finalEdge = numEdges-1;
				int currentEdgeCheck = j+1;
				bool found = false;

				//find the starting and final edges for a node
				while((currentEdgeCheck < numNodes)&&(!found)){
					if((sentNodes+currentEdgeCheck)->numEdgesOut > 0){
						found = true;
						finalEdge = (sentNodes+currentEdgeCheck)->firstEdge - 1;
					}
					currentEdgeCheck++;
				}
				//change the rank for the node
				for(int k = startEdge; k <= finalEdge; k++){
					(sentNodes+j)->rankNext += ((sentNodes+((sentEdges+k)->endNodeLabel)-1)->rankCurr)/((sentNodes+((sentEdges+k)->endNodeLabel)-1)->numEdgesIn);
				}
			}
		}

		//check for convergence, as well as sets the new value for the current rank
		//and resets the value for nextRank
		converged = 1;
		for(int k = 0; k < numNodes; k++){
			float nextRank = ((1-dampFact)/(numNodes))+dampFact*((sentNodes+k)->rankNext);

			float convergenceCompare = (sentNodes+k)->rankCurr - nextRank;
			if((convergenceCompare >= convergenceFactor)||(convergenceCompare <= (-1)*convergenceFactor)){
				converged = 0;
			}
			(sentNodes+k)->rankCurr = nextRank;
			(sentNodes+k)->rankNext = 0;
			totSum += (sentNodes+k)->rankCurr;
		}
	}

	//print out the final rank for all of the pages
	cout << "The end Status for the nodes " << endl;
	for(int k = 0; k < numNodes; k++){
		(sentNodes+k)->rankCurr = (sentNodes+k)->rankCurr/totSum;
		cout << k+1 << " node rank = " << (sentNodes+k)->rankCurr << endl;
	}
}




int main(int argc, char *argv[]){
	if(argc < 3){
		cout << "Not enough command line arguments" << endl;
		return(0);
	}
	ifstream DimacsFile;
	DimacsFile.open(argv[1]); //right now wikipedia.txt or rmat15.txt
	if(!DimacsFile.is_open()){
		//file was not opened
		cout << "File not successfully opened" << endl;
		return 0;
	}else{
		//begin reading in the file and constructing a CSR representation of the graph
		string nextLine;
		int type, totalEdges, totalNodes;    ///used to determine the type of problem, type 1 = page rank
		graphNode* Nodes;
		graphEdge* Edges;
		list<edgeData> edgeList;
		int currentLocation = 0; //used to determine the current location in the Edges list
		while(getline(DimacsFile, nextLine)){
			if(nextLine.length() == 0){

			}else{

				char lineStart = nextLine.at(0);
				//parse the string
				if(lineStart == 'a'){
					//an edge line
					string stringReader;
					int i = 2;

					//read start node
					int startNode = 0;
					while(nextLine.at(i) != ' '){
						startNode = startNode*10 + (nextLine.at(i) - '0');
						i++;
					}
					i++;

					//read end node
					int endNode = 0;
					while((nextLine.at(i) != ' ')&&(i < nextLine.size())){
						endNode = endNode*10 + (nextLine.at(i) - '0');
						i++;
					}
					i++;


					//read weight
					int weight = 0;
					while(i < nextLine.size()){
						weight = weight*10 + (nextLine.at(i) - '0');
						i++;
					}

					//add edge to the list of edges
					edgeData newEdge;
					newEdge.weight = weight;
					newEdge.start = startNode;
					newEdge.end = endNode;

					edgeList.push_back(newEdge);
						
					currentLocation++;

				}else if(lineStart == 'c'){
				//do nothing with comment lines, only for human readability

				}else if(lineStart == 'p'){
				//start with finding the format type of the problem (only supports page rank)

					//reads in the Format
					string stringReader;
					int i = 2;
					while(nextLine.at(i) != ' '){
						stringReader += nextLine.at(i);
						i++;
					}
					i++;
					//sets the type of problem
					if(stringReader.compare("sp")){
						type = 1;
					}

					//reads in the total number of nodes
					totalNodes = 0;
					while(nextLine.at(i) != ' '){
						totalNodes = totalNodes*10 + (nextLine.at(i) - '0');
						i++;
					}
					i++;

					//reads in the total number of edges
					totalEdges = 0;
					while(i < nextLine.length()){
						totalEdges = totalEdges*10 + (nextLine.at(i) - '0');
						i++;
					}

//					//malloc space for lists
					Nodes = (graphNode*)malloc(totalNodes*sizeof(graphNode));
					Edges = (graphEdge*)malloc(totalEdges*sizeof(graphEdge));

					//add nodes labels to the list of Nodes
					for(int j = 0; j < totalNodes; j++){
						graphNode addNode;
						addNode.numEdgesIn = 0;
						addNode.numEdgesOut = 0;
						addNode.rankCurr = 0;
						addNode.rankNext = 0;
						addNode.firstEdge = -1;
						*(Nodes + j) = addNode;
					}							


				}else{	
					//Line does not start with a valid character
					cout << "Valid character not found at a line" << endl;
				}
			}
		}
		if(type = 1){
			if(strcmp(argv[2], "push") == 0){
				//sorts the list of edges and creates a CSR graph of the edges
				edgeList.sort(edgeCompare);
				int i = 0;
				while(0 < edgeList.size()){
					edgeData theEdge = edgeList.front();

					graphEdge newEdge;
					newEdge.endNodeLabel = theEdge.end;
					newEdge.weight = theEdge.weight;
					*(Edges+i) = newEdge;

					if((Nodes+theEdge.start-1)->numEdgesOut == 0){
						(Nodes+theEdge.start-1)->firstEdge = i;
					}	

					(Nodes+(theEdge.start-1))->numEdgesOut += 1;
					(Nodes+(theEdge.end-1))->numEdgesIn += 1;

					edgeList.pop_front();
					i++;
				}
				pushStylePR(Nodes, Edges, totalNodes, totalEdges);



			}else if(strcmp(argv[2], "pull") == 0){
				//sorts the list of edges and creates a csr graph of the transpose edges.
				edgeList.sort(edgeCompareTranspose);
				int i = 0;
				while(0 < edgeList.size()){
					edgeData theEdge = edgeList.front();

					graphEdge newEdge;
					newEdge.endNodeLabel = theEdge.start;
					newEdge.weight = theEdge.weight;
					*(Edges+i) = newEdge;

					if((Nodes+theEdge.end-1)->numEdgesOut == 0){
						(Nodes+theEdge.end-1)->firstEdge = i;
					}	

					(Nodes+(theEdge.end-1))->numEdgesOut += 1;
					(Nodes+(theEdge.start-1))->numEdgesIn += 1;

					edgeList.pop_front();
					i++;
				}
				pullStylePR(Nodes, Edges, totalNodes, totalEdges);


			}else{
				cout << "Invalid command Line argument of style of pagerank" << endl;
				return(0);
			}
		}
		DimacsFile.close();
	}
}

