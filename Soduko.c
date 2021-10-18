#include "soduko.h"

//-------------------------Main functions--------------------------

void playGame(TPNODE* activeRoot, PlayersList* activePlayers, PlayersList* winnersPlayers) {
	//Recursively function that moving in order on the tree player and calling to fill board function.
	//In addition updating the active and winners lists.

	if (activeRoot == NULL || activeRoot->player == NULL) return;

	playGame(activeRoot->left, activePlayers, winnersPlayers);

	printf("\n%s It's your turn!\n", activeRoot->player->player.name);

	if (FillBoard(activeRoot->player->player.board, activeRoot->player->player.posMat) == FINISH_FAILURE) {
		printf("\n%s selections led to duplications\n\n", activeRoot->player->player.name);
		deletePlayerFromPlayersList(activePlayers, activeRoot->player);
		activeRoot->player = NULL;
	}
	else {
		printf("\n%s board filled successfully\n\n", activeRoot->player->player.name);
		insertNodeToEndPlayersList(winnersPlayers, activeRoot->player);
		activeRoot->player = NULL;
	}

	playGame(activeRoot->right, activePlayers, winnersPlayers);
}

Array*** PossibleDigits(short sudokuBoard[][SIZE]) {
	//Creating the possibilities matrix.
	//Getting a possibilites array for each cell in matrix.

	Array*** possibleMat;
	Cell tempCell;
	int i, j, size;

	possibleMat = (Array***)malloc(SIZE * sizeof(Array**));
	checkAllocation(possibleMat);
	allocatePossibleMat(possibleMat);

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (sudokuBoard[i][j] == EMPTY_CELL) {
				tempCell.row = i;
				tempCell.col = j;
				possibleMat[i][j]->arr = getPossibleValues(sudokuBoard, tempCell, &size);
				possibleMat[i][j]->size = size;
			}
			else {
				possibleMat[i][j]->arr = NULL;
				possibleMat[i][j]->size = 0;
			}
		}
	}

	return possibleMat;
}

int OneStage(short board[][SIZE], Array*** possibilities, int* x, int* y) {
	//This function checks if there is an emtpy cell with 1 possibilty and put it in board.
	//After each placement updating board and possibilities matrix.
	//Return in x, y variables the row and col for the cell with the minimum possibilities.
	//And return if success , failed, or not finished status.

	int i, j, minSize = SIZE + 1;
	short val;
	bool minFlag = false, runFlag = true;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (possibilities[i][j]->size == 1 && board[i][j] == EMPTY_CELL) { //If cell has 1 possibility --> put it in board.
				val = possibilities[i][j]->arr[0];
				board[i][j] = val;
				free(possibilities[i][j]->arr);
				possibilities[i][j]->arr = NULL;
				possibilities[i][j]->size = 0;
				updatePossibilities(possibilities, val, i, j); //call for updating row,col,small matrix possibilities.
				i = j = 0;
			}
		}
	}

	if (isBoardFull(board)) //check if board is full.
		return FINISH_SUCCESS;

	return checkMatAndMin(board, possibilities, &minSize, x, y);
}

int FillBoard(short board[][SIZE], Array*** possibilities) {
	//The function calling oneStage in a loop, and if there is more than 1 possibilites in a cell so getting the choice from the user,
	//and calling for updating board and possibilities mat.
	//At the end returns if finished or fail.

	Array* minPos;
	int gameStatus, x, y;
	short i, selectedVal;
	bool checkSelect = true;

	gameStatus = OneStage(board, possibilities, &x, &y);

	while (gameStatus == NOT_FINISH) {
		printBoard(board);
		minPos = possibilities[x][y];

		printf("Cell number [%d,%d] currently holds the minimum number of possible values, select one of the below: \n", x, y);
		for (i = 0; i < minPos->size; i++) {
			printf("%hi. %hi\n", i + 1, minPos->arr[i]);
		}

		scanf("%hi", &selectedVal);
		checkSelect = checkSelectedVal(minPos, selectedVal);

		while (!checkSelect) { //If wrong choice from user --> continue get the val and check.
			printf("Wrong choice, try again...\n");
			scanf("%hi", &selectedVal);
			checkSelect = checkSelectedVal(minPos, selectedVal);
		}

		printf("Updating cell %d with value %hi\n\n", x * SIZE + y, selectedVal);
		board[x][y] = selectedVal;
		updatePossibilities(possibilities, selectedVal, x, y); //call for updating row,col,small matrix possibilities.

		gameStatus = OneStage(board, possibilities, &x, &y);
	}

	return gameStatus;
}

void checkAllocation(void* arr) {
	//Check for allocation success.
	if (arr == NULL) {
		printf("Allocation Failed!");
		exit(1);
	}
}


//-------------------------Board utilities--------------------------

int randNum(int range) {
	//Getting a range and return a random number except 0 in that range.
	int N;

	srand(time(NULL));
	N = rand() % range;
	while (N == 0)
		N = rand() % range;

	return N;
}

void createBoard(short board[][SIZE]) {
	//Creating random board.
	int N, i, randCellPos, size;
	short* arr;
	short val;
	ListCell posCells = createCoordsList();
	Cell tmpCell;

	N = randNum(21); //getting number of cells to fill (1-20).

	for (i = 0; i < N; i++) {
		randCellPos = randNum(82 - i);
		tmpCell = getCellByPos(posCells, randCellPos);

		arr = getPossibleValues(board, tmpCell, &size); //get arr of possible values for that cell.

		// rand a number from possible values arr and put it in board.
		val = arr[randNum(size + 1) - 1];
		board[tmpCell.row][tmpCell.col] = val;

		RemoveNodeToPlaceInCellsList(&posCells, randCellPos); // Remove the cell from the list.

		free(arr);
	}

	freeListCell(&posCells);
}

void initBoard(short board[][SIZE]) {
	//Initialize all board cells to -1.
	int i, j;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			board[i][j] = EMPTY_CELL;
		}
	}
}

bool isBoardFull(short board[][SIZE]) {
	//Checking if board is full --> no empty cell.
	int i, j;
	bool flag = true;

	for (i = 0; i < SIZE && flag; i++) {
		for (j = 0; j < SIZE && flag; j++) {
			if (board[i][j] == EMPTY_CELL)
				flag = false;
		}
	}

	return flag;
}

Cell getCellByPos(ListCell posCells, int pos) {
	//Getting a position in cells list and returns that cell by position.
	CellNode* curr;
	curr = posCells.head;
	while (curr != NULL && pos > 1)
	{
		curr = curr->next;
		pos--;
	}

	return curr->data;

}

short* getPossibleValues(short board[][SIZE], Cell cell, int* size) {
	//Getting a cell on board.
	//Using counting sort checking for all possibilies numbers to that cell, and returns an allocated array with the possibilites.
	int phySize = 0, row, col, i, j = 0;
	short* res;
	short arr[SIZE] = { 0 };

	get3x3Mat(&row, &col, cell.row, cell.col);

	checkPossible3x3Mat(board, arr, row, col);
	checkPossibleRow(board, arr, cell.row);
	checkPopssibleCol(board, arr, cell.col);

	for (i = 0; i < SIZE; i++) {
		if (arr[i] == 0) phySize++;
	}

	res = (short*)malloc(phySize * sizeof(short));
	checkAllocation(res);

	for (i = 0; i < SIZE; i++) {
		if (arr[i] == 0) {
			res[j] = i + 1;
			j++;
		}
	}

	*size = phySize;
	return res;
}

void checkPossible3x3Mat(short board[][SIZE], short arr[], int row, int col) {
	//Checks for empty cell in the getting 3x3 matrix and add +1 to the counting arr.
	int i, j;

	for (i = row; i <= row + 2; i++) {
		for (j = col; j <= col + 2; j++) {
			if (board[i][j] != EMPTY_CELL)
				(arr[board[i][j] - 1])++;
		}
	}
}

void checkPossibleRow(short board[][SIZE], short arr[], int row) {
	//Checks for empty cell in the getting row and add +1 to the counting arr.
	int j;

	for (j = 0; j < SIZE; j++) {
		if (board[row][j] != EMPTY_CELL)
			(arr[board[row][j] - 1])++;
	}
}

void checkPopssibleCol(short board[][SIZE], short arr[], int col) {
	//Checks for empty cell in the getting col and add +1 to the counting arr.
	int i;

	for (i = 0; i < SIZE; i++) {
		if (board[i][col] != EMPTY_CELL)
			(arr[board[i][col] - 1])++;
	}
}

void get3x3Mat(int* i, int* j, int row, int col) {
	//Getting a row and col, and returns the starters row and col to the small 3x3 matrix.
	if (row >= 0 && row <= 2) {
		if (col >= 0 && col <= 2) *j = 0;
		else if (col >= 3 && col <= 5) *j = 3;
		else *j = 6;
		*i = 0;
	}
	else if (row >= 3 && row <= 5) {
		if (col >= 0 && col <= 2) *j = 0;
		else if (col >= 3 && col <= 5) *j = 3;
		else *j = 6;
		*i = 3;
	}
	else {
		if (col >= 0 && col <= 2) *j = 0;
		else if (col >= 3 && col <= 5) *j = 3;
		else *j = 6;
		*i = 6;
	}
}

bool checkForFailure(short board[][SIZE], Array*** possibilities) {
	//This function checking for failure --> empty cell with no possibilities.
	//Return true is failure found.
	int i, j;
	bool flag = false;

	for (i = 0; i < SIZE && !flag; i++) {
		for (j = 0; j < SIZE && !flag; j++) {
			if (board[i][j] == EMPTY_CELL && possibilities[i][j]->size == 0)
				flag = true;
		}
	}

	return flag;
}

int getNumOfFilledCells(short board[][SIZE]) {
	//Getting a board and return the number of filled cells in board.
	int i, j, count = 0;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (board[i][j] != EMPTY_CELL)
				count++;
		}
	}

	return count;
}

void printBoard(short board[][SIZE]) {
	//Printing board.
	int i, j, k;

	for (i = 0; i < SIZE; i++) {
		if (i == 0) { // print the cols numbers.
			printf("  ");
			for (k = 0; k < SIZE; k++) {
				if (k == 0 || k == 3 || k == 6) {
					printf("|");
				}
				printf("%2d", k);
			}
			printf("\n");
		}

		if (i == 0 || i == 3 || i == 6) {
			printf("-----------------------\n");
		}

		for (j = 0; j < SIZE; j++) {
			if (j == 0) //print the row nubmers.
				printf("%2d|", i);

			if (board[i][j] != EMPTY_CELL) {
				if (j == 2 || j == 5)
					printf("%2hi|", board[i][j]);
				else printf("%2hi", board[i][j]);
			}
			else {
				if (j == 2 || j == 5)
					printf("  |");
				else printf("  ");
			}
		}
		printf("\n");
	}
	printf("-----------------------\n");
	printf("\n");
}


//-------------------Possibilities mat utilities-------------------

void updatePossibilities(Array*** possibilities, short val, int i, int j) {
	//This function called after putting a number in the board.
	//And responsible for updating and remove the number from all possibilites arrays with that number.
	int row, col;

	get3x3Mat(&row, &col, i, j);

	deleteFrom3x3Mat(possibilities, val, row, col);
	deleteFromRow(possibilities, val, i);
	deleteFromCol(possibilities, val, j);
}

void deleteFrom3x3Mat(Array*** possibilities, short val, int row, int col) {
	//deleting val from all possibilites arrays in that small 3x3 col and row matrix.
	int i, j;
	Array* temp;
	for (i = row; i <= row + 2; i++) {
		for (j = col; j <= col + 2; j++) {
			if (possibilities[i][j]->size != 0) {
				temp = possibilities[i][j];
				checkAndDeleteVal(temp, val);
			}
		}
	}
}

void deleteFromRow(Array*** possibilities, short val, int row) {
	//deleting val from all possibilites arrays in that row.
	int j;
	Array* temp;
	for (j = 0; j < SIZE; j++) {
		if (possibilities[row][j]->size != 0) {
			temp = possibilities[row][j];
			checkAndDeleteVal(temp, val);
		}
	}
}

void deleteFromCol(Array*** possibilities, short val, int col) {
	//deleting val from all possibilites arrays in that col.
	int i;
	Array* temp;
	for (i = 0; i < SIZE; i++) {
		if (possibilities[i][col]->size != 0) {
			temp = possibilities[i][col];
			checkAndDeleteVal(temp, val);
		}
	}
}

void checkAndDeleteVal(Array* posCell, short val) {
	//Checks if val is in array --> if found delete it from the array and reallocate.
	int i;
	bool flag = false;

	if (posCell->size == 1 && posCell->arr[0] == val) { // if has only one possibility - the one we will delete.
		free(posCell->arr);
		posCell->arr = NULL;
		posCell->size = 0;
		return;
	}

	for (i = 0; i < posCell->size - 1; i++) {
		if (posCell->arr[i] == val) { //pushing the val to the end of the array.
			posCell->arr[i] = posCell->arr[i + 1];
			flag = true;
		}
		else if (flag)
			posCell->arr[i] = posCell->arr[i + 1];
	}

	if (posCell->arr[posCell->size - 1] == val || flag) { //if found val in array -> reallocate.
		posCell->arr = (short*)realloc(posCell->arr, ((posCell->size) - 1) * sizeof(short));
		checkAllocation(posCell->arr);
		posCell->size -= 1;
	}
}

bool checkSelectedVal(Array* posCell, short val) {
	//Return true if val is in posCell array, otherwise returns false.
	int i;

	for (i = 0; i < posCell->size; i++) {
		if (posCell->arr[i] == val)
			return true;
	}

	return false;
}

void allocatePossibleMat(Array*** possibleMat) {
	//Allocates possibilities matrix.
	int i, j;

	for (i = 0; i < SIZE; i++) {
		possibleMat[i] = (Array**)malloc(SIZE * sizeof(Array*));
		checkAllocation(possibleMat[i]);
		for (j = 0; j < SIZE; j++) {
			possibleMat[i][j] = (Array*)malloc(sizeof(Array));
			checkAllocation(possibleMat[i][j]);
		}
	}
}

int checkMatAndMin(short board[][SIZE], Array*** possibilities, int* minSize, int* minRow, int* minCol) {
	//This function checks if there is failure (empty cell with no possibilities)
	//and meanwhile updating the cell with the minimum possibilities in minRow and minCol variables.
	int i, j;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (board[i][j] == EMPTY_CELL && possibilities[i][j]->size == 0)
				return FINISH_FAILURE;
			if (board[i][j] == EMPTY_CELL && possibilities[i][j]->size > 1 && possibilities[i][j]->size < *minSize) {
				*minSize = possibilities[i][j]->size;
				*minRow = i;
				*minCol = j;
			}
		}
	}

	return NOT_FINISH;
}

void freePossibleMat(Array*** possibleMat) {
	//Free possibilities matrix.
	int i, j;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (possibleMat[i][j]->arr != NULL)
				free(possibleMat[i][j]->arr);
			free(possibleMat[i][j]);
		}
		free(possibleMat[i]);
	}

	free(possibleMat);
}


//----------------------Players list utilities----------------------

PlayersList getListOfPlayers(int numOfPlayers) {
	//This function creating the list of players.
	PlayersList res;
	int  i;

	makeEmptyPlayersList(&res);

	getchar();

	for (i = 0; i < numOfPlayers; i++) {
		printf("Enter player %d name\n", i + 1);
		insertDataToEndPlayersList(&res);
	}

	return res;
}

void makeEmptyPlayersList(PlayersList* lst) {
	//Points the head and tail to Null - Empty list.
	lst->head = NULL;
	lst->tail = NULL;
}

void insertDataToEndPlayersList(PlayersList* lst) {
	//Insert new Tail with the data and put it at the end of the list.
	PlayerNode* newTail;
	newTail = createNewPlayerListNode(NULL);
	insertNodeToEndPlayersList(lst, newTail);
}

PlayerNode* createNewPlayerListNode(PlayerNode* next) {
	//Creating new list node.
	PlayerNode* res;

	res = (PlayerNode*)malloc(sizeof(PlayerNode));
	checkAllocation(res);

	res->player = createPlayer();

	res->next = next;

	return res;
}

void insertNodeToEndPlayersList(PlayersList* lst, PlayerNode* tail) {
	//Inserting the node to the end of the list.
	if (isEmptyPlayersList(lst) == true)
		lst->head = lst->tail = tail;
	else
	{
		lst->tail->next = tail;
		lst->tail = tail;
	}
	tail->next = NULL;
}

Player createPlayer() {
	//Return new player with name, board and possibleDigits mat.
	Player p;

	gets(p.name);

	initBoard(p.board);

	createBoard(p.board);
	p.posMat = PossibleDigits(p.board);

	while (checkForFailure(p.board, p.posMat)) { // If creating board did'nt success.
		createBoard(p.board);
		p.posMat = PossibleDigits(p.board);
	}

	return p;
}

bool isEmptyPlayersList(PlayersList* lst) {
	//Checking if the list is empty.
	if (lst->head == NULL)
		return true;
	else
		return false;
}

PlayerNode** createPlayersPointersArray(PlayersList lst, int size, int* newSize) {
	//This function allocates a pointers array, calling for merging the array and extend the array with NULLs if needed.
	//Returns the new pointers array and the new size in newSize variable.

	PlayerNode** playersPointers;
	PlayerNode* curr = lst.head;
	int i;

	playersPointers = (PlayerNode**)malloc(size * sizeof(PlayerNode*));
	checkAllocation(playersPointers);

	for (i = 0; i < size; i++) { //initialize each pointer to a node in player list.
		playersPointers[i] = curr;
		curr = curr->next;
	}

	mergeSort(playersPointers, size);

	*newSize = pow(2, ceil(log2(size + 1))) - 1; //extending

	playersPointers = (PlayerNode**)realloc(playersPointers, (*newSize) * sizeof(PlayerNode*));

	for (i = size; i < (*newSize); i++) { //adding NULLs if needed.
		playersPointers[i] = NULL;
	}

	return playersPointers;
}

void mergeSort(PlayerNode** playersPointers, int size) {
	//Recursively merge sorting by num of filled cells (if equal so by names).
	PlayerNode** tmpArr = NULL;

	if (size <= 1)
		return;

	mergeSort(playersPointers, size / 2);
	mergeSort(playersPointers + size / 2, size - size / 2);
	tmpArr = merge(playersPointers, size / 2, playersPointers + size / 2, size - size / 2);

	if (tmpArr) {
		copyArr(playersPointers, tmpArr, size); // copy values from tmpArr to arr
		free(tmpArr);
	}
	else {
		printf("Allocation Failed!");
		exit(1); // end program immediately with code 1 (indicating an 
	}
}

PlayerNode** merge(PlayerNode** a1, int size1, PlayerNode** a2, int size2) {
	//Merging the two player nodes arrays by num of filled cells (if equal so by names).
	int ind1, ind2, resInd;
	int p1FilledCells, p2FilledCells, returnVal;

	PlayerNode** res = (PlayerNode**)malloc((size1 + size2) * sizeof(PlayerNode*));
	checkAllocation(res);

	if (res) {
		ind1 = ind2 = resInd = 0;
		while ((ind1 < size1) && (ind2 < size2)) {
			//getting filled cells for each player in the array:
			p1FilledCells = getNumOfFilledCells(a1[ind1]->player.board);
			p2FilledCells = getNumOfFilledCells(a2[ind2]->player.board);

			if (p1FilledCells < p2FilledCells) {
				res[resInd] = a1[ind1];
				ind1++;
			}
			else if (p1FilledCells > p2FilledCells) {
				res[resInd] = a2[ind2];
				ind2++;
			}
			else { // num of filled cells equal --> check by name.
				returnVal = strcmp(a1[ind1]->player.name, a2[ind2]->player.name);
				if (returnVal <= 0) {
					res[resInd] = a1[ind1];
					ind1++;
				}
				else {
					res[resInd] = a2[ind2];
					ind2++;
				}
			}
			resInd++;
		}

		//if there is more pointers to players in some array:
		while (ind1 < size1) {
			res[resInd] = a1[ind1];
			ind1++;
			resInd++;
		}
		while (ind2 < size2) {
			res[resInd] = a2[ind2];
			ind2++;
			resInd++;
		}
	}
	return res;
}

void copyArr(PlayerNode** playersPointers, PlayerNode** tmpArr, int size) {
	//Copying tmpArr to playersPointers array.
	int i;

	for (i = 0; i < size; i++) {
		playersPointers[i] = tmpArr[i];
	}
}

void deletePlayerFromPlayersList(PlayersList* lst, PlayerNode* player) {
	//Delete player from players list
	PlayerNode* curr = lst->head;

	if (curr == player) {
		lst->head = curr->next;
		if (lst->tail == player) //If the only player in list.
			lst->tail = NULL;
	}
	else {
		while (curr->next != NULL) {
			if (curr->next == player) {
				curr->next = player->next;
			}
			curr = curr->next;
		}

		if (curr->next == NULL) //if is at the end of the list.
			lst->tail = curr;
	}

	freePossibleMat(player->player.posMat);
	free(player);
	player = NULL;
}

void printWinners(PlayersList lst) {
	//This function getting the winner list and printing it (if there is winners).
	if (lst.head == NULL) {
		printf("There is no winners! everybody losers!!!\n\n");
		return;
	}

	printf("The winners are:\n");
	PlayerNode* curr = lst.head;

	while (curr != NULL) {
		printf("%s\n", curr->player.name);
		printBoard(curr->player.board);
		curr = curr->next;
	}
}

void freePlayerList(PlayersList* lst) {
	//Free players list using recursive function.
	freePlayerListHelper(lst->head);
}

void freePlayerListHelper(PlayerNode* head) {
	//free the list in recursion.
	if (head != NULL) {
		freePlayerListHelper(head->next);
		free(head);
	}
}


//---------------------Players tree utilities------------------------

PTREE BuildTreeFromArray(PlayerNode** playersPointers, int size) {
	//Building a tree from playersPointers array using recursive function.
	PTREE res;

	res.root = BuildTreeFromArrayHelper(playersPointers, size);
	return res;
}

TPNODE* BuildTreeFromArrayHelper(PlayerNode** playersPointers, int  size) {
	//This function getting a players pointers array and recursively creating a tree from that array. 
	TPNODE* resRoot;
	int midSize, lSize, rSize;

	midSize = lSize = size / 2;
	rSize = size - lSize - 1;

	if (size == 0)
		return NULL;

	else {
		resRoot = createTreeNode(playersPointers[midSize]);
		resRoot->left = BuildTreeFromArrayHelper(playersPointers, lSize);
		resRoot->right = BuildTreeFromArrayHelper(playersPointers + midSize + 1, rSize);
		return resRoot;
	}
}

TPNODE* createTreeNode(PlayerNode* playerPointer) {
	//Getting a player pointer and creating a new tree node with pointer to the player pointer.
	TPNODE* res = (TPNODE*)malloc(sizeof(TPNODE));
	res->player = playerPointer;
	res->left = NULL;
	res->right = NULL;
	return res;
}

void printTreeInorder(PTREE tr) {
	//printing tree using recursive function.
	printTreeInorderHelper(tr.root);
}

void printTreeInorderHelper(TPNODE* root) {
	//Printing the tree in order recursively.
	if (root == NULL || root->player == NULL) return;

	printTreeInorderHelper(root->left);
	printf("%s\n", root->player->player.name);
	printBoard(root->player->player.board);
	printTreeInorderHelper(root->right);
}

void freeTree(PTREE tr) {
	//freeing tree by using recursive function.
	freeTreeHelper(tr.root);
	tr.root = NULL;
}

void freeTreeHelper(TPNODE* root) {
	//recursive function for free tree.
	if (root == NULL) return;

	freeTreeHelper(root->left);
	freeTreeHelper(root->right);
	free(root);
}


//----------------------Cells list utilities------------------------

ListCell createCoordsList() {
	//Creating a cell list that contain all board location (row, col) --> 81 nodes.
	int i, j;
	ListCell posCells;
	Cell tmpCell;
	makeEmptyCellsList(&posCells);

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			tmpCell.row = i;
			tmpCell.col = j;
			insertDataToEndCellsList(&posCells, tmpCell);
		}
	}

	return posCells;
}

void makeEmptyCellsList(ListCell* lst) {
	//Points the head and tail to Null - Empty list.
	lst->head = NULL;
	lst->tail = NULL;
}

void insertDataToEndCellsList(ListCell* lst, Cell cell) {
	//Insert new Tail with the data and put it at the end of the list.
	CellNode* newTail;
	newTail = createNewCellsListNode(cell, NULL);
	insertNodeToEndCellsList(lst, newTail);
}

CellNode* createNewCellsListNode(Cell cell, CellNode* next) {
	//Creating new list node.
	CellNode* res;

	res = (CellNode*)malloc(sizeof(CellNode));
	checkAllocation(res);

	res->data = cell;
	res->next = next;

	return res;
}

void insertNodeToEndCellsList(ListCell* lst, CellNode* tail) {
	//Inserting the node to the end of the list.
	if (isEmptyCellsList(lst) == true)
		lst->head = lst->tail = tail;
	else
	{
		lst->tail->next = tail;
		lst->tail = tail;
	}
	tail->next = NULL;
}

bool isEmptyCellsList(ListCell* lst) {
	//Checking if the list is empty.
	if (lst->head == NULL)
		return true;
	else
		return false;
}

void insertNodeToStartCellsList(ListCell* lst, CellNode* head) {
	//Inserting the node to the start of the list.
	if (isEmptyCellsList(lst) == true)
	{
		head->next = NULL;
		lst->head = lst->tail = head;
	}
	else
	{
		head->next = lst->head;
		lst->head = head;
	}
}

bool RemoveNodeToPlaceInCellsList(ListCell* lst, int i) {
	//Removing the i node in list. returns true if removing succeed otherwise returns false.
	CellNode* after;
	CellNode* tmp;
	after = getListCellNode(lst, i - 1);
	if (after == NULL && i > 0) // Illegal location
		return false;
	else if (i == 1) { // Start of list
		tmp = lst->head;
		lst->head = lst->head->next;
		free(tmp);
	}
	else if (after->next->next == NULL) { // End of list
		tmp = lst->tail;
		lst->tail = after;
		after->next = NULL;
		free(tmp);
	}
	else // “Middle”
	{
		tmp = after->next;
		after->next = tmp->next;
		free(tmp);
	}
	return true;
}

CellNode* getListCellNode(ListCell* lst, int i) {
	//Returns the cell node by his location in list (i).
	CellNode* curr;
	curr = lst->head;
	while (curr != NULL && i > 1) {
		curr = curr->next;
		i--;
	}
	return curr;
}

void freeListCell(ListCell* lst) {
	//Free the list.
	freeListCellHelper(lst->head);
}

void freeListCellHelper(CellNode* head) {
	//free the list in recursion.
	if (head != NULL) {
		freeListCellHelper(head->next);
		free(head);
	}
}


