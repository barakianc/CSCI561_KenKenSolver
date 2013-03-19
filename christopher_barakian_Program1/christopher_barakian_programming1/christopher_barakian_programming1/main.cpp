/*the main file for the KenKen solver*/
#include "main.h"

int main(int argc,char* argv[]){
	KenKenPuzzle ourPuzzle;
	gOutPut.open(argv[2]);
	float runTime;
	if(parseKenKenInput(&ourPuzzle,argv[1])){
		cout << "Reading in Puzzle!" << endl;
		//if the creation of the puzzle was successful then we need to generate
		//the domains for each of the cages
		if(checkBoard(&ourPuzzle)){
			int test = 0;
		}
		genCageDomains(&ourPuzzle);

		//now that the domains of each cage is generated we will perform the backtracking algorithm by recursively
		//going through the cages and back tracking when none of its solutions can be chosen
		stack<KenKenCage> Cages;
		while(!ourPuzzle.cages.empty()){
			Cages.push(ourPuzzle.cages.front());
			ourPuzzle.cages.pop();
		}

		cout << "Solving Puzzle..." << endl;
		backTrack(&ourPuzzle,Cages,argv[2]);
		gOutPut.close();
		runTime = ((float)clock())/CLOCKS_PER_SEC;
		cout.precision(5);
		cout << "Puzzle was solved in " << runTime << " seconds" << endl;
		cout << "Hit Enter to continue!" << endl;
		int test = cin.get();
	}

	return 0;
}

//initialize a KenKenPuzzle
bool initKenKenPuzzle(KenKenPuzzle *puzzle){
	if(puzzle != NULL){
		puzzle = new KenKenPuzzle();
		return true;
	}
	else{
		cerr << "Bad pointer!"<< endl;
		return false;
	}
}
//parse the input file
bool parseKenKenInput(KenKenPuzzle *puzzle,char *filepath){
	if(puzzle != NULL && filepath != NULL){
		fstream inputFile;
		inputFile.open(filepath);
		if(inputFile.is_open()){
			int maxCell = 0;
			while(inputFile.good()){
				string cageString;
				getline(inputFile,cageString);
				/*TODO: for each string create a new cage for the puzzle and 
				keep track of highes cell to determinethe size of the board*/
				KenKenCage tempCage;
				int stringPos = 0;
				char* tempStr = new char[10];
				//read in the cages target value
				while(cageString[stringPos] != ' '){
					tempStr[stringPos] = cageString[stringPos];
					stringPos++;
				}
				tempCage.targetValue = atoi(tempStr);
				stringPos++;
				//get the cages operation
				switch(cageString[stringPos]){
				case('+'):
					{
						tempCage.op = ADDITION;
						break;
					}
				case('-'):
					{
						tempCage.op = SUBTRACTION;
						break;
					}
				case('*'):
					{
						tempCage.op = MULTIPLY;
						break;
					}
				case('/'):
					{
						tempCage.op = DEVIDE;
						break;
					}
				case('.'):
					{
						tempCage.op = EQUAL;
						break;
					}
				}
				stringPos+=2;
				//read in the cells this cage covers
				int tempCharPos = 0;
				delete(tempStr);
				tempStr = new char[10];
				for(stringPos; stringPos < cageString.length(); stringPos++){
					if(cageString[stringPos] == ' '){
						tempCage.cells.push_back(atoi(tempStr));
						tempCharPos = 0;
						if(atoi(tempStr) > maxCell){
							maxCell = atoi(tempStr);
						}
						delete(tempStr);
						tempStr = new char[10];
					}
					else if(stringPos == cageString.length()-1){
						tempStr[tempCharPos] = cageString[stringPos];
						tempCage.cells.push_back(atoi(tempStr));
						tempCharPos = 0;
						if(atoi(tempStr) > maxCell){
							maxCell = atoi(tempStr);
						}
						delete(tempStr);
					}
					else{
						tempStr[tempCharPos] = cageString[stringPos];
						tempCharPos++;
					}
				}
				tempCage.numCells = tempCage.cells.size();
				puzzle->cages.push(tempCage);
			}
			puzzle->board = new int*[(maxCell+1)];
			for(int i = 0; i < (maxCell+1);i++){
				puzzle->board[i] = new int[(maxCell+1)];
			}
			puzzle->size = (int)sqrt((maxCell+1.0));
			initBoard(puzzle->board,puzzle->size);
			inputFile.close();
		}
		else{
			cerr << filepath << endl << "File failed to open!!"<< endl;
			inputFile.close();
			return false;
		}
		return true;
	}
	else{
		cerr << "Bad pointer!" << endl;
		return false;
	}
}

//check if the current state of the board is valid
bool checkBoard(KenKenPuzzle* puzzle){
	//check the rows
	for(int i = 0; i < puzzle->size ; i++){
		std::set<int> r1,r2;
		for(int j = 0; j < puzzle->size; j++){
			if(puzzle->board[i][j] != 0){
				if(r1.find(puzzle->board[i][j]) != r1.end()){
					return false;
				}
				else{
					r1.insert(puzzle->board[i][j]);
				}
			}

			if(puzzle->board[j][i] != 0){
				if(r2.find(puzzle->board[j][i]) != r2.end()){
					return false;
				}
				else{
					r2.insert(puzzle->board[j][i]);
				}
			}
		}
		r1.clear();
		r2.clear();
	}
	return true;
}

//generate the domains for each of the cages in the puzzle
bool genCageDomains(KenKenPuzzle* puzzle){
	queue<KenKenCage> tempqueue;
	while(!puzzle->cages.empty()){
		KenKenCage tempCage = puzzle->cages.front();
		switch(tempCage.op){
		case cEQL:
			{
				//equals cages have only 1 solution
				list<int> tempList (1,tempCage.targetValue);
				tempCage.resDomain.insert(tempList);
				puzzle->cages.pop();
				tempqueue.push(tempCage);
				break;
			}
		case cADD:
			{
				int* solutions = new int[tempCage.numCells];
				for(int i = 0; i < tempCage.numCells; i++){
					solutions[i] = 1;
				}
				bool moreSols = true;
				while(moreSols){
					bool increment = false;
					//we need to increment to find all possible solutions to this cage
					for(int i = tempCage.numCells-1; i >=0;i--){
						int testSum = 0;
						//first test if the current cell values is a solution
						for(int j = 0; j < tempCage.numCells; j++){
							testSum += solutions[j];
						}
						

						//if so added it to the cages domain
						if(testSum == tempCage.targetValue){
							list<int> tempList;
							for(int j = 0; j < tempCage.numCells; j++){
								tempList.push_back(solutions[j]);
							}
							//check if the solution can be excluded because it will violate the game board
							list<int>::const_iterator lIT1,lIT2;
							lIT2 = tempCage.cells.begin();
							for(lIT1 = tempList.begin(); lIT1 != tempList.end(); lIT1++){
								int r = (int)floorf((*lIT2)/puzzle->size);	//row
								int c = (*lIT2)%(puzzle->size);				//column
								puzzle->board[r][c] = *lIT1;
								lIT2++;
							}
							//check if the solutions violate the board
							if(checkBoard(puzzle)){
								tempCage.resDomain.insert(tempList);
							}

							//clear the board
							list<int>::const_iterator iter;
							for(iter = tempCage.cells.begin(); iter != tempCage.cells.end(); iter++){
								int r = (int)floorf((*iter)/puzzle->size);	//row
								int c = (*iter)%(puzzle->size);				//column
								puzzle->board[r][c] = 0;
							}

						}
						//Test to see if we have finished
						if(i == 0 && solutions[i] == puzzle->size && increment){
							increment = false;
							moreSols = false;
							break;
						}
						else if(i != tempCage.numCells-1 && increment){
							increment = false;
							if(solutions[i] == puzzle->size){
								increment = true;
								solutions[i] = 1;
							}
							else{
								solutions[i]++;
							}
						}
						else if(i == tempCage.numCells-1){
							if(solutions[i] == puzzle->size){
								increment = true;
								solutions[i] = 1;
							}
							else{
								solutions[i]++;
							}
						}
					}
				}
				puzzle->cages.pop();
				tempqueue.push(tempCage);
				free(solutions);
				break;
			}
		case cSUB:
			{
				int* solutions = new int[2];
				solutions[0] = 1;
				solutions[1] = 1;
				bool moreSols = true;
				while(moreSols){
					bool increment = false;
					//we need to increment to find all possible solutions to this cage
					for(int i = tempCage.numCells-1; i >=0;i--){
						//first test if the current cell values is a solution
						//if so added it to the cages domain
						if(solutions[0] - solutions[1] == tempCage.targetValue || solutions[1] - solutions[0] == tempCage.targetValue){
							list<int> tempList;
							for(int j = 0; j < tempCage.numCells; j++){
								tempList.push_back(solutions[j]);
							}
							tempCage.resDomain.insert(tempList);
						}
						//Test to see if we have finished
						if(i == 0 && solutions[i] == puzzle->size && increment){
							increment = false;
							moreSols = false;
							break;
						}
						else if(i != tempCage.numCells-1 && increment){
							increment = false;
							if(solutions[i] == puzzle->size){
								increment = true;
								solutions[i] = 1;
							}
							else{
								solutions[i]++;
							}
						}
						else if(i == tempCage.numCells-1){
							if(solutions[i] == puzzle->size){
								increment = true;
								solutions[i] = 1;
							}
							else{
								solutions[i]++;
							}
						}
					}
				}
				puzzle->cages.pop();
				tempqueue.push(tempCage);
				free(solutions);
				break;
			}
		case cMULT:
			{
				int* solutions = new int[tempCage.numCells];
				for(int i = 0; i < tempCage.numCells; i++){
					solutions[i] = 1;
				}
				bool moreSols = true;
				while(moreSols){
					bool increment = false;
					//we need to increment to find all possible solutions to this cage
					for(int i = tempCage.numCells-1; i >=0;i--){
						int testMult = 1;
						//first test if the current cell values is a solution
						for(int j = 0; j < tempCage.numCells; j++){
							testMult *= solutions[j];
						}
						//if so added it to the cages domain
						if(testMult == tempCage.targetValue){
							list<int> tempList;
							for(int j = 0; j < tempCage.numCells; j++){
								tempList.push_back(solutions[j]);
							}
							//check if the solution can be excluded because it will violate the game board
							list<int>::const_iterator lIT1,lIT2;
							lIT2 = tempCage.cells.begin();
							for(lIT1 = tempList.begin(); lIT1 != tempList.end(); lIT1++){
								int r = (int)floorf((*lIT2)/puzzle->size);	//row
								int c = (*lIT2)%(puzzle->size);				//column
								puzzle->board[r][c] = *lIT1;
								lIT2++;
							}
							//check if the solutions violate the board
							if(checkBoard(puzzle)){
								tempCage.resDomain.insert(tempList);
							}

							//clear the board
							list<int>::const_iterator iter;
							for(iter = tempCage.cells.begin(); iter != tempCage.cells.end(); iter++){
								int r = (int)floorf((*iter)/puzzle->size);	//row
								int c = (*iter)%(puzzle->size);				//column
								puzzle->board[r][c] = 0;
							}
						}
						//Test to see if we have finished
						if(i == 0 && solutions[i] == puzzle->size && increment){
							increment = false;
							moreSols = false;
							break;
						}
						else if(i != tempCage.numCells-1 && increment){
							increment = false;
							if(solutions[i] == puzzle->size){
								increment = true;
								solutions[i] = 1;
							}
							else{
								solutions[i]++;
							}
						}
						else if(i == tempCage.numCells-1){
							if(solutions[i] == puzzle->size){
								increment = true;
								solutions[i] = 1;
							}
							else{
								solutions[i]++;
							}
						}
					}
				}
				puzzle->cages.pop();
				tempqueue.push(tempCage);
				free(solutions);
				break;
			}
		case cDEV:
			{
				int* solutions = new int[2];
				solutions[0] = 1;
				solutions[1] = 1;
				bool moreSols = true;
				while(moreSols){
					bool increment = false;
					//we need to increment to find all possible solutions to this cage
					for(int i = tempCage.numCells-1; i >=0;i--){
						//first test if the current cell values is a solution
						//if so added it to the cages domain
						if(((float)solutions[0] / (float)solutions[1]) == (float)tempCage.targetValue || ((float)solutions[1] / (float)solutions[0]) == (float)tempCage.targetValue){
							list<int> tempList;
							for(int j = 0; j < tempCage.numCells; j++){
								tempList.push_back(solutions[j]);
							}
							tempCage.resDomain.insert(tempList);
						}
						//Test to see if we have finished
						if(i == 0 && solutions[i] == puzzle->size && increment){
							increment = false;
							moreSols = false;
							break;
						}
						else if(i != tempCage.numCells-1 && increment){
							increment = false;
							if(solutions[i] == puzzle->size){
								increment = true;
								solutions[i] = 1;
							}
							else{
								solutions[i]++;
							}
						}
						else if(i == tempCage.numCells-1){
							if(solutions[i] == puzzle->size){
								increment = true;
								solutions[i] = 1;
							}
							else{
								solutions[i]++;
							}
						}
					}
				}
				puzzle->cages.pop();
				tempqueue.push(tempCage);
				free(solutions);
				break;
			}
		}
	}
	puzzle->cages = tempqueue;
	return true;
}

//init the puzzle board to all 0
bool initBoard(int** puzzleBoard,int size){
	for(int i = 0; i <size ;i++){
		for(int j = 0; j < size; j++){
			puzzleBoard[i][j] = 0;
		}
	}
	return true;
}

//outputs the board to the specified output file
bool outputBoard(KenKenPuzzle* puzzle){
	if(puzzle != NULL && gOutPut.is_open()){
		if(gOutPut.is_open()){
			for(int i = 0; i < puzzle->size; i++){
				for(int j = 0; j < puzzle->size; j++){
					gOutPut << puzzle->board[i][j] << " ";
				}
				gOutPut << endl;
			}
			gOutPut << endl;
		}
		else{
			cerr << "Could not open file!" << endl;
			return false;
		}
	}
	else{
		cerr << "Bad Pointer!" << endl;
		return false;
	}
	return true;
}

//run the backtracking algorithm for the cage variables and print out the 
//board when you find a solution, stop when no more solutions can be found
bool backTrack(KenKenPuzzle* puzzle,stack<KenKenCage> Cages,char *filepath){
	if(puzzle != NULL && filepath != NULL){
		if(Cages.size() <3){
			int test = 0;
		}
		//if the stack is empty then a solution has ben found and print the board
		if(Cages.empty()){
			outputBoard(puzzle);
			return true;
		}
		else{
			KenKenCage tempCage = Cages.top();
			Cages.pop();
			set<list<int>>::iterator sIT;

			//iterate over all solutions
			for(sIT = tempCage.resDomain.begin(); sIT != tempCage.resDomain.end(); sIT++){
				//insert the next solution into the board
				list<int>::const_iterator lIT1,lIT2;
				lIT2 = tempCage.cells.begin();
				for(lIT1 = sIT->begin(); lIT1 != (*sIT).end(); lIT1++){
					int r = (int)floorf((*lIT2)/puzzle->size);	//row
					int c = (*lIT2)%(puzzle->size);				//column
					puzzle->board[r][c] = *lIT1;
					lIT2++;
				}

				//outputBoardtoScreen(puzzle);
				//check if solution fits in our graph
				if(checkBoard(puzzle)){
					backTrack(puzzle,Cages,filepath);
				}
			}

			//after all solutions tried reset board and back track
			list<int>::const_iterator iter;
			for(iter = tempCage.cells.begin(); iter != tempCage.cells.end(); iter++){
				int r = (int)floorf((*iter)/puzzle->size);	//row
				int c = (*iter)%(puzzle->size);				//column
				puzzle->board[r][c] = 0;
			}
		}
	}
	else{
		cerr << "Bad Pointer!" << endl;
		return false;
	}
	return true;
}

bool outputBoardtoScreen(KenKenPuzzle* puzzle){
	for(int i = 0; i < puzzle->size; i++){
		for(int j = 0; j < puzzle->size; j++){
			clog << puzzle->board[i][j] << " ";
		}
		clog << endl;
	}
	clog << endl;
	return true;
}