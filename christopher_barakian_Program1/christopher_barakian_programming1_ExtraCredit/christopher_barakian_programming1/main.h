/*main header file include basice library imports*/
#include<iostream>
#include<fstream>
#include<string>
#include<stdlib.h>
#include<queue>
#include<stack>
#include<list>
#include<set>
#include<time.h>
using namespace std;

/*definitions*/
#define ADDITION 0;
#define SUBTRACTION 1;
#define MULTIPLY 2;
#define DEVIDE 3;
#define EQUAL 4;
/*end definitions*/

/*typedefs*/
typedef int Operation;
/*end type def*/

/*Constants*/
const int cADD = ADDITION;
const int cSUB = SUBTRACTION;
const int cMULT = MULTIPLY;
const int cDEV = DEVIDE;
const int cEQL = EQUAL;
/*End of constants*/

/*Global Definitions*/
ofstream gOutPut;
/*End of Globals*/

/*struct defs*/
struct KenKenCage{
	int numCells;
	int targetValue;
	Operation op;
	std::list<int> cells; //contains the cells that make up this cage 
	std::set<list<int>>	resDomain;		//contains all possible solutions to this cage
	//int* cellValues;//contains the calues in the cells
};
struct KenKenPuzzle{
	std::queue<KenKenCage> cages;		//list of cages for the puzzle;
	int**			  board;		//represent the puzzle board
	int				  size;			//this size of a nxn board

};
/*end struct defs*/

/*function definitions*/
bool initKenKenPuzzle(KenKenPuzzle* puzzle);
bool parseKenKenInput(KenKenPuzzle* puzzle,char *filepath);
bool genCageDomains(KenKenPuzzle* puzzle);
bool checkBoard(KenKenPuzzle* puzzle);
bool initBoard(int** puzzleBoard,int size);
bool outputBoard(KenKenPuzzle* puzzle);
bool outputBoardtoScreen(KenKenPuzzle* puzzle);
bool operator<(const KenKenCage& lhs,const KenKenCage& rhs);
bool backTrack(KenKenPuzzle* puzzle,priority_queue<KenKenCage, vector<KenKenCage>,less<KenKenCage>> Cages,char *filepath);
/*end function definitions*/