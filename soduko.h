#ifndef SODUKO_H
#define SODUKO_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define SIZE 9
#define MAX_NAME 100
#define EMPTY_CELL -1
#define NOT_FINISH 0
#define FINISH_SUCCESS 1
#define FINISH_FAILURE -1


//Structs:
typedef struct cell
{
	unsigned int row;
	unsigned int col;
}Cell;

typedef struct _Array
{
	short* arr;
	unsigned short size;
} Array;

typedef struct player
{
	char name[MAX_NAME];
	short board[SIZE][SIZE];
	struct _Array*** posMat;
}Player;

typedef struct cell_node
{
	Cell data;
	struct cell_node* next;
}CellNode;

typedef struct list_cell
{
	CellNode* head;
	CellNode* tail;
}ListCell;

typedef struct player_node
{
	Player player;
	struct player_node* next;
}PlayerNode;

typedef struct players_list
{
	PlayerNode* head;
	PlayerNode* tail;
}PlayersList;

typedef struct tpnode
{
	PlayerNode* player;
	struct tpnode* left;
	struct tpnode* right;

}TPNODE;

typedef struct players_tree
{
	TPNODE* root;
}PTREE;

//Functions:

//-------------------------Main functions--------------------------
void playGame(TPNODE* activeRoot, PlayersList* activePlayers, PlayersList* winnersPlayers);
Array*** PossibleDigits(short sudokuBoard[][SIZE]);
int OneStage(short board[][SIZE], Array*** possibilities, int* x, int* y);
int FillBoard(short board[][SIZE], Array*** possibilities);
void checkAllocation(void* arr);

//-------------------------Board utilities--------------------------
int randNum(int range);
void createBoard(short board[][SIZE]);
void initBoard(short board[][SIZE]);
bool isBoardFull(short board[][SIZE]);
Cell getCellByPos(ListCell posCells, int pos);
short* getPossibleValues(short board[][SIZE], Cell cell, int* size);
void checkPossible3x3Mat(short board[][SIZE], short arr[], int row, int col);
void checkPossibleRow(short board[][SIZE], short arr[], int row);
void checkPopssibleCol(short board[][SIZE], short arr[], int col);
void get3x3Mat(int* i, int* j, int row, int col);
bool checkForFailure(short board[][SIZE], Array*** possibilities);
int getNumOfFilledCells(short board[][SIZE]);
void printBoard(short board[][SIZE]);

//-------------------Possibilities mat utilities-------------------
void updatePossibilities(Array*** possibilities, short val, int i, int j);
void deleteFrom3x3Mat(Array*** possibilities, short val, int row, int col);
void deleteFromRow(Array*** possibilities, short val, int row);
void deleteFromCol(Array*** possibilities, short val, int col);
void checkAndDeleteVal(Array* posCell, short val);
bool checkSelectedVal(Array* posCell, short val);
void allocatePossibleMat(Array*** possibleMat);
int checkMatAndMin(short board[][SIZE], Array*** possibilities, int* minSize, int* minRow, int* minCol);
void freePossibleMat(Array*** possibleMat);

//----------------------Players list utilities----------------------
PlayersList getListOfPlayers(int numOfPlayers);
void makeEmptyPlayersList(PlayersList* lst);
void insertDataToEndPlayersList(PlayersList* lst);
PlayerNode* createNewPlayerListNode(PlayerNode* next);
void insertNodeToEndPlayersList(PlayersList* lst, PlayerNode* tail);
Player createPlayer();
bool isEmptyPlayersList(PlayersList* lst);
PlayerNode** createPlayersPointersArray(PlayersList lst, int size, int* newSize);
void mergeSort(PlayerNode** playersPointers, int size);
PlayerNode** merge(PlayerNode** a1, int size1, PlayerNode** a2, int size2);
void copyArr(PlayerNode** playersPointers, PlayerNode** tmpArr, int size);
void deletePlayerFromPlayersList(PlayersList* lst, PlayerNode* player);
void printWinners(PlayersList lst);
void freePlayerList(PlayersList* lst);
void freePlayerListHelper(PlayerNode* head);

//---------------------Players tree utilities------------------------
PTREE BuildTreeFromArray(PlayerNode** playersPointers, int size);
TPNODE* BuildTreeFromArrayHelper(PlayerNode** playersPointers, int  size);
TPNODE* createTreeNode(PlayerNode* playerPointer);
void printTreeInorder(PTREE tr);
void printTreeInorderHelper(TPNODE* root);
void freeTree(PTREE tr);
void freeTreeHelper(TPNODE* root);

//----------------------Cells list utilities-------------------------
ListCell createCoordsList();
void makeEmptyCellsList(ListCell* lst);
void insertDataToEndCellsList(ListCell* lst, Cell cell);
CellNode* createNewCellsListNode(Cell cell, CellNode* next);
void insertNodeToEndCellsList(ListCell* lst, CellNode* tail);
bool isEmptyCellsList(ListCell* lst);
void insertNodeToStartCellsList(ListCell* lst, CellNode* head);
bool RemoveNodeToPlaceInCellsList(ListCell* lst, int i);
CellNode* getListCellNode(ListCell* lst, int i);
void freeListCell(ListCell* lst);
void freeListCellHelper(CellNode* head);

#endif // !SODUKO_H

