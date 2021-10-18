#ifndef BOARD_H
#include "main.h"

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

#endif // !BOARD_H

