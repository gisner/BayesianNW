//Giselle Isner 12/12 Fordham AI 

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <math.h>
//#include <windows.h>

using namespace std;

int numNodes;
int numRows;
int numProbsInBN;
int getNumNodes();
int getNumRows();
void readFromFile();
bool compareMatrices(vector<char> oneM, vector<char> twoM);

void createTruthTableJD();
void processProbsTable();
void printBayesNW();
void findJointDist(vector<char> oneRow);

struct node 
{
	string nodeName;
	vector<string> parents;
	vector<int> parentsIndices;	
	vector<double> probs;
	vector< vector<char> > truthTable;
		
};
vector<node*> arrayOfNodes; //array of all nodes in nw
void populateTruthTableNodes(node* oneNode, int rows, int cols);
void addParentNode(node* oneNode,string parentName);


int main()
{	

    numNodes = 0;
    numRows = 0;
    numProbsInBN = 0;
  
    readFromFile();
    numNodes = getNumNodes();
    
    if(numNodes > 0)
    {
	numRows = getNumRows();	
	processProbsTable();
	printBayesNW();
	createTruthTableJD(); 
	
    	cout << "Number of multiplications needed to calc joint dist = " << (numNodes-1)*numRows << endl;
	cout << "(number of mults needed per row/combination = " << numNodes-1 << ")\n\n";
	cout << "Number of combos/rows in Joint Distribution  = " << numRows << endl;
    	cout << "\nNumber of nodes in bayesian network = " << numNodes << endl;
	cout << "\nSavings of " << numRows << " vs. " << numProbsInBN << endl;
   	
    }
    return 0;
}
/***********************************
*
* printBayesNW(): outputs Bayesian Net
*
*************************************/
void printBayesNW()
{
	cout << "__________________" << endl;
	cout << "BAYESIAN NETWORK:" << endl;
	cout << "__________________" << endl;

	for(int i = 0; i < arrayOfNodes.size(); i++)
	{
		cout << "  \n\nNODE # " << i << ": " << arrayOfNodes[i]->nodeName << endl;
		cout << "------------------------" << endl;

		if(arrayOfNodes[i]->probs.size() == 1)	//if node does not have parents
		{ 
			cout << "P(" << arrayOfNodes[i]->nodeName << ") " << endl;
			cout << arrayOfNodes[i]->probs[0] << "\n\n";		
		}
		else
		{
			for(int pars = 0; pars < arrayOfNodes[i]->parents.size(); pars++)
				cout << setw(12) << arrayOfNodes[i]->parents[pars];
			cout <<  setw(15) << "P( " << arrayOfNodes[i]->nodeName << "| ";
			for(int pars = 0; pars < arrayOfNodes[i]->parents.size(); pars++)
				cout << arrayOfNodes[i]->parents[pars] << " ";	
			cout << ")" << endl;
			for(int row = 0; row <  arrayOfNodes[i]->probs.size(); row++)	
			{
				for(int col = 0; col <  arrayOfNodes[i]->parents.size(); col++)
					cout <<  setw(12) << arrayOfNodes[i]->truthTable[row][col];
				cout <<  setw(20) << arrayOfNodes[i]->probs[row];
				cout << endl;	
			}
			cout << "\n\n\n";
		}
	}
}
/*****************************************************************************
*
* processProbsTable(): creates truth table for each node if node has parents 
*	               calls populateTruthTableNodes()
*
*******************************************************************************/
void processProbsTable()
{

	for(int i = 0; i < arrayOfNodes.size(); i++)
		if(arrayOfNodes[i]->probs.size() > 1)
			populateTruthTableNodes(arrayOfNodes[i], arrayOfNodes[i]->probs.size(),arrayOfNodes[i]->parents.size());			
    	

}

void populateTruthTableNodes(node* oneNode, int rows, int cols)
{
	vector<char> tempVec;
	for(int i = 0; i < cols; i++)
		tempVec.push_back('T');
	for(int i = 0; i < rows; i++)
		oneNode->truthTable.push_back(tempVec);
		
	int exp= 1;
    	for( int i = 0;  i < cols; i++)
    	{
        	for(int j= 0;  j< rows; j++)
        	{
	   		int val = 1- j/exp % 2;
	   		if(val)
				oneNode->truthTable[j][cols-1-i] = 'T';
			else
				oneNode->truthTable[j][cols-1-i] = 'F'; 	 	 
		}
        	exp *= 2;
	}	
}
/***************************************
*
* readFromFile(): inputs data from file
*
***************************************/
void readFromFile()
{
	string myFile  = "";
	cout << "ENTER FILE NAME:\n";
	cin >> myFile;
	
	ifstream bayesfile(myFile); 
	
	if(!bayesfile)
	{
		cout << "FILE ERROR opening file"  << myFile << "!!!\n"; 
		exit(1);
	}

	char *delim = (char *)" ,()\t";
	char *tkn;
	string bayesline; 
		
	while(getline(bayesfile, bayesline)) 
	{
			
		node *topNode; 
			
 		if(bayesline.empty())
			continue;

		const char *temp = bayesline.c_str();
		tkn = strtok((char*)temp,delim);
			
		
		if(tkn != NULL && strcmp(tkn,"END") !=0 && strcmp(tkn,"END\r\n") !=0  && strcmp(tkn,"END\r") !=0 && strcmp(tkn,"END\n") !=0 && strcmp(tkn,"\r\n") !=0 && strcmp(tkn,"\n") !=0 && strcmp(tkn,"\r") !=0 && strcmp(tkn,"") !=0)
		{
			topNode = new node; //first "word" on line has to be name of a node
			topNode->nodeName = tkn;
		}
		else 
			continue;

		tkn = strtok(NULL,delim);
		

		while(tkn != NULL)
		{	
			if(!atof(tkn) && strcmp(tkn,"NONE") != 0) //any non-number is name of parent
			{
				topNode->parents.push_back(tkn);
				addParentNode(topNode, tkn);
			}
			else if(atof(tkn)) //any number is a probability
			{
				topNode->probs.push_back(atof(tkn));	
				numProbsInBN++;
			}
			tkn = strtok(NULL,delim);
		}
		arrayOfNodes.push_back(topNode); 	
		
	}	
}
/******************************************************************
*
* addParentNode(): each node in arrayOfNodes vector has a list
*		 of indices (from same arrayOfNodes vector) of its parents' 	
*		 nodes. This will assist in future processing of
*		 joint distribution. My initial implementation
*                saved entire "parent node"(linked-list) rather 
*                than a list of indices. I later felt indices 
*                would suffice.
*
*******************************************************************/
void addParentNode(node* oneNode, string parentName)
{

	for(int i = 0; i < arrayOfNodes.size(); i++)
		if(parentName == arrayOfNodes[i]->nodeName)
			oneNode->parentsIndices.push_back(i);
}
/******************************************************************
*   
*  createTruthTableJD(): creates truth table for purpose of outputting
*                        joint distribution. 
*                        Calls findJointDist() with one combination/row
*                        in truth table
*
***********************************************************************/
void createTruthTableJD()
{
	vector< vector<char> > oneRow;
	vector<char> tempVec;
	for(int i = 0; i < numNodes; i++)
		tempVec.push_back('T');
	for(int i = 0; i < numRows; i++)
		oneRow.push_back(tempVec);
	
    	int exp= 1;
   	 for( int i = 0;  i < numNodes; i++)
   	 {
        	for( int j= 0;  j< numRows; j++ )
        	{
	   		int val = 1- j/exp % 2;
           		if(val)
				oneRow[j][numNodes-1-i] = 'T';
	   		else
				oneRow[j][numNodes-1-i] = 'F';			
        	}
        	exp *= 2;
    	}

	cout << endl;
	cout << "____________________" << endl;
	cout << "JOINT DISTRIBUTION:" << endl;
	cout << "____________________\n" << endl;

    	for(int i = 0; i < arrayOfNodes.size(); i++)
		cout << arrayOfNodes[i]->nodeName << "  ";
   	cout << "\n-------------\n\n";

    	for(int i = 0; i < numRows;i++)
		findJointDist(oneRow[i]);
			

}
/******************************************
*
* findJointDist(): outputs prob per row
*
*******************************************/
void findJointDist(vector<char> oneRow)
{
	vector<double> probsToMult; 
		
	for(int i = 0; i < oneRow.size(); i++)
	{
		//if no parents - one prob
		if(arrayOfNodes[i]->probs.size() == 1)
		{
			if(oneRow[i] == 'T')
				probsToMult.push_back(arrayOfNodes[i]->probs[0]); 
			else
				probsToMult.push_back(1-arrayOfNodes[i]->probs[0]);
		}
		//if parents - more than one prob
		else
		{
			vector<char> parentVals;
			int index = 0;

			for(int pars = 0; pars < arrayOfNodes[i]->parentsIndices.size(); pars++)
				parentVals.push_back(oneRow[arrayOfNodes[i]->parentsIndices[pars]]);
			
			for(int nodeTT = 0; nodeTT < arrayOfNodes[i]->truthTable.size(); nodeTT++)
				if(compareMatrices(arrayOfNodes[i]->truthTable[nodeTT], parentVals))
				{
					index = nodeTT;
					break;
				}
			
			if(oneRow[i] == 'T')
				probsToMult.push_back(arrayOfNodes[i]->probs[index]);
			else
				probsToMult.push_back(1 - arrayOfNodes[i]->probs[index]);
		}
	}

	//print data
	for(int i = 0; i < oneRow.size(); i++)
		cout << oneRow[i] << "  ";
	cout << endl;

	double finalVal = 1; 

	for(int i = 0; i <  probsToMult.size(); i++)
	{
		cout << setprecision(3) << fixed << probsToMult[i] << "  "; 
		finalVal *= probsToMult[i]; 
		
	}
	
	cout << " = " << setprecision(20) << fixed << finalVal;
	cout << "\n\n\n";
}

bool compareMatrices(vector<char> oneM, vector<char> twoM)
{
	
	for(int i = 0; i < oneM.size(); i++)
		if(oneM[i] != twoM[i])
			return false;
			
	return true;

}
int getNumNodes()
{
	return arrayOfNodes.size();
}
int getNumRows()
{
	return pow(2,numNodes);   

}









