#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define MAX_PILE_SIZE 10
#define COMPUTER 1
#define MAN 2
#define NONE 0

using namespace std;

char old_pile[MAX_PILE_SIZE+1][MAX_PILE_SIZE+1];
char new_pile[MAX_PILE_SIZE+1][MAX_PILE_SIZE+1];

void get_initial_parameters(string &player_name, int &pile_count);
void create_initial_piles(int pile_count, int piles[]);
void display_player_details(string player_name);
void display_game(string player_name, int pile_count, int piles[], int player);
bool game_complete(int pile_count, int piles[]);
void computer_move(int pile_count, int piles[]);
void player_move(int pile_count, int piles[]);
void announce_winner(int current_player, string player_name);
void fill_pile(int pile_count, int piles[]);

int main()
{
	// list of variables
	string player_name;
	int pile_count;
	int fid;	
	int status;
	char *args[2];
	int piles[11];
	int current_player;

	// seed for random number
	srand((unsigned)time(NULL));

	// system call to clear to screen
	args[0] = (char *)malloc(strlen("/usr/bin/clear") * sizeof(char) + 1);
	strcpy(args[0], "/usr/bin/clear");
	args[2] = NULL;

	// child executes the clear screen command
	fid = fork();
	if (fid == 0) {
		execv(args[0], args);
		exit(0);
	}
	wait(&status);

	get_initial_parameters(player_name, pile_count);

	create_initial_piles(pile_count, piles);

	//display_game(player_name, pile_count, piles, NONE);

	current_player = COMPUTER;

	fill_pile(pile_count, piles);

	while (!game_complete(pile_count, piles)) {
		fid = fork();
		if (fid == 0) {
			execv(args[0], args);
			exit(0);
		}
		wait(&status);

		computer_move(pile_count, piles);
		display_game(player_name, pile_count, piles, COMPUTER);
		current_player = MAN;
		if (game_complete(pile_count, piles)) {
			break;
		}
		player_move(pile_count, piles);
		fill_pile(pile_count, piles);
		current_player = COMPUTER;
	}

	announce_winner(current_player, player_name);

	return 0;
}

/*
 * this function is used to take initial parameter required
 * in the game like player name and the number of nim piles
 * @param -
 * 	player_name = the name of the human player
 *	pile_count = the number of nim piles the 
 *		human player want
 *
 */
void get_initial_parameters(string &player_name, int &pile_count)
{
	// get the name of the player
	cout << "Welcome to the game." << endl;
	cout << "Please enter you name: ";
	cin >> player_name;

	// get the count of nim piles
	cout << player_name << " plase select the number of piles.(between 2 to 10): ";
	cin >> pile_count;
}

/*
 * this function is used to display the player details on
 * the top margin. The current details showing are -
 * 1. player name
 *
 * @param -
 * 	player_name = name of the human player
 *
 */
void display_player_details(string player_name)
{
	cout << "Player Name: " << player_name << endl;
	cout << endl;
}

/*
 * This function is used to create a pile such that 
 * there is absolute chance that the first player will 
 * win, if both sides play optimally.
 * @param - 
 *	pile_count = number of nim piles
 *	piles[] = array that stores the size of
 * 		each pile.
 */
void create_initial_piles(int pile_count, int piles[])
{
	int i, x, y;

	// y is used to track the current xor of
	// the generated values.
	y = 0;

	for (i = 1; i < pile_count; ++i) {
		x = rand() % MAX_PILE_SIZE;
		piles[i] = x; 
		y ^= x;
	}

	// put the last value such that the xor is not zero
	piles[pile_count] = (y+1) % (MAX_PILE_SIZE + 1);
}

/*
 * this function is uesd to display the current nim 
 * configuration of the game, or simply the current
 * game screen
 * @param - 
 * 	player_name = the name of the cirrent man player
 *	pile_count = the number of the nim piles
 *	piles[] = array that stores the number of stones 
 *		  in the pile.
 *	player = check for MAN, COMPUTER, or NONE (initial display)
 */
void display_game(string player_name, int pile_count, int piles[], int player)
{
	int i, j, n;
			
	// the top bar that has player name.
	display_player_details(player_name);
	
	
	fill_pile(pile_count, piles);

	n = 2 * pile_count - 1;
	
	// displaying the current piles
	for (i = 0; i < n+5; ++i) {
		putchar('-');
	} 
	cout << "        ";
	for (i = 0; i < n+5; ++i) {
		putchar('-');
	} 
	cout << endl;

	for (j = MAX_PILE_SIZE-1; j >= 0; --j) {
		cout << "| ";
		for (i = 1; i <= pile_count; ++i) {
			cout << old_pile[i][j] << " ";
		} 
		if (j == MAX_PILE_SIZE / 2)cout << " |  --->  | ";
		else cout << " |        | ";
		for (i = 1; i <= pile_count; ++i) {
			cout << new_pile[i][j] << " ";
		}
		cout << " |";
		cout << endl;
	}

	// base of the nim pile
	for (i = 0; i < n+5; ++i) {
		putchar('-');
	}
	cout << "        ";
	for (i = 0; i < n+5; ++i) {
		putchar('-');
	} 
	cout << endl << endl << endl;
}

/*
 * this function is use to check whether the game is complete
 * or not. 
 * @param - 
 * 	pile_count = number of piles that the player select
 *	piles[] = array that stores the size of current piles
 * game finishes when all pile size is zero.
 *
 */
bool game_complete(int pile_count, int piles[])
{
	int i;

	// traverse over all piles to check
	// whether any pile has stones left or not	
	for (i = 1; i <= pile_count; ++i) {
		if (piles[i] != 0) {
			// in case if any pile has stone left
			return false;
		}
	}

	// if pile count of all pile is 0
	return true;
}

/*
 * This function is use to announce the winner of the game
 * @param - 
 *	current_player = stores who made the last move
 *	player_name = name of current human player
 *
 */
void announce_winner(int current_player, string player_name)
{
	if (current_player == MAN) {
		// if the player that made the last move was computer.
		cout << "Oh! So sorry, seems like computer wins. :P" << endl;
	} else if (current_player == COMPUTER) {
		// if the player that made the last move was human.
		cout << "Congo " << player_name << ", you really beatup the computer this time." << endl;
	}
	cout << "\n\n\n\n";
}

/*
 * This function is used for a player to make its
 * move. All necessary conditions must be checked to
 * prevent cheating hack.
 * @param - 
 *	pile_count = the total number of nim piles
 *	piles[] = array that contain stones in each pile.
 *
 */
void player_move(int pile_count, int piles[])
{
	int pile_number;
	int stone_count;
	bool done;

	// take the pile number
	done = false;
	while (!done) {
		cout << "Enter the pile number: ";
		cin >> pile_number;

		// if the pile number is out of bound
		if (pile_number < 1 || pile_number > pile_count) {
			cout << "Invalid pile number. Please enter a valid number" << endl;
			continue;
		}

		// if there are no stones on the selected pile
		if (piles[pile_number] == 0) {
			cout << "Please select another pile as there is no stone on this pile" << endl;
			continue;
		}
	
		// task complete
		done = true;
	}
		
	// take the number of stones to be taken
	done = false;
	while (!done) {
		cout << "Enter the number of stones to be taken: ";
		cin >> stone_count;

		// if the stones are <= 0 or greater than available stones.
		if (stone_count < 1 || stone_count > piles[pile_number]) {
			cout << "Invalid number of stones." << endl;
			continue;
		}

		// task complete
		done = true;
	}

	// modify the number of stones in the pile.
	piles[pile_number] -= stone_count;
}

/* 
 * this function is used for computer to make its move
 * so that it puts the man player in a loosing move
 * @param - 
 *	pile_count = number of piles that user selected
 *	piles[] = count of stones in each pile.
 * 
 */
void computer_move(int pile_count, int piles[])
{
	int i, j, k;
	int total_xor;
	int current_xor;

	// let us calculate the total xor
	total_xor = 0;
	for (i = 1; i <= pile_count; ++i) {
		total_xor ^= piles[i];
	}

	// now to select from where to get stones
	for (i = 1; i <= pile_count; ++i) {
		// xor of all number leaving the current number
		// equals the xor of all numbers xored with current number
		current_xor = total_xor ^ piles[i];

		// we need to make total xor 0 for man player to loose
		if (piles[i] > current_xor) {
			piles[i] = current_xor;
			break;
		}
	}
}	

/*
 * this function is used to memorise the last pile and 
 * then fill up the current pile by the values of
 * stones.
 * @param - 
 *	pile_count = the number of nim piles.
 *	piles[] = array to store the number of stones 
 *		  in each pile. 
 */
void fill_pile(int pile_count, int piles[])
{
	int i, j;

	for (i = 1; i <= pile_count; ++i) {
		for (j = 0; j < MAX_PILE_SIZE; ++j) {
			old_pile[i][j] = new_pile[i][j];
		}
	}

	for (i = 1; i <= pile_count; ++i) {
		for (j = 0; j < piles[i]; ++j) {
			new_pile[i][j] = '*';
		} 
		for (; j < MAX_PILE_SIZE; ++j) {
			new_pile[i][j] = ' ';
		}
	}
}
