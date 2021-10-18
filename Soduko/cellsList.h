#ifndef CELLSLIST_H
#include "main.h"

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

#endif // !CELLSLIST_H
