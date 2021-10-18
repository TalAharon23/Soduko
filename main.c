#include "soduko.h"

// ***********Soduko Game***********
//Tal Aharon 208458414 
//Idan Cohen 208270595

void main() {
	Array*** possible_solutions;
	PTREE activePlayersTree;

	int numOfPlayers, n;
	PlayersList activePlayers, winnersPlayers;
	PlayerNode** playersPointersArr;

	printf("***********Soduko Game***********\n\n");

	printf("Please enter the number of players\n");
	scanf("%d", &numOfPlayers);

	makeEmptyPlayersList(&winnersPlayers);

	activePlayers = getListOfPlayers(numOfPlayers);
	playersPointersArr = createPlayersPointersArray(activePlayers, numOfPlayers, &n);
	activePlayersTree = BuildTreeFromArray(playersPointersArr, n);

	playGame(activePlayersTree.root, &activePlayers, &winnersPlayers);

	printWinners(winnersPlayers);

	freeTree(activePlayersTree);
	freePlayerList(&activePlayers);
	free(playersPointersArr);
}