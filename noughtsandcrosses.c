#pragma region Libaries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "utilities.h"
#pragma endregion

#pragma region Definitions
#define GLOBAL_DEBUG 1
#define GRID_LENGTH 3
#define GRID_HEIGHT 3
#define MAIN_TIMEOUT_SECONDS 2
#define HORIZONTAL_AND_VERTICAL_WIN_AMOUNT 3
#define TEMP_FIXED_BUFFER_FOR_STRING 50
#pragma endregion

#pragma region Exit Codes
#define EXIT_MEMORY_ALLOCATE_FAILURE 244
#pragma endregion

int GAME_ROWS[GRID_LENGTH * GRID_HEIGHT];

const char PLAYER_TYPE_NAMES[3][50] = {
	"None", "Crosses", "Noughts"
};

enum PLAYER_TYPES
{
	player_none = 0,
	player_crosses = 1,
	player_noughts = 2
};

int PLAYER_GAME_TYPE = player_crosses;
int COMPUTER_GAME_TYPE = player_noughts;

int TOTAL_PLAYER_WON_GAMES = 0;
int TOTAL_COMPUTER_WON_GAMES = 0;

void take_player_game_input();
void print_game_rows();

void reprint_game_to_screen() {
	flush_screen();
	print_game_rows();
}

void choice_menu() {
	flush_screen();

	int side_choice = 0;

	if (TOTAL_COMPUTER_WON_GAMES != 0 || TOTAL_PLAYER_WON_GAMES != 0) {

		printf("-----[Stats]-----\n\n");
		printf("Players wins [%i]\nComputer Wins [%i]\n\n", TOTAL_PLAYER_WON_GAMES, TOTAL_COMPUTER_WON_GAMES);
		printf("-----------------\n\n");
	}

	printf("Choose which side you want to play on [1] Crosses, [2] Noughts...");

	(void)scanf_s("%i", &side_choice);

	if (side_choice > 2 || side_choice < 0) {
		printf("\nSide choice must be between 0 and 2. Restarting");
		wait_seconds(MAIN_TIMEOUT_SECONDS);
		choice_menu();
		return;
	}

	PLAYER_GAME_TYPE = side_choice;
	COMPUTER_GAME_TYPE = side_choice == 2 ? 1 : 2;

	reprint_game_to_screen();
	take_player_game_input();
}

void resync_game_rows() {
	for (int i = 0; i < GRID_HEIGHT * GRID_LENGTH; i++) {
		GAME_ROWS[i] = 0;
	}
}

int* get_row_indexes() {
	int total_length = GRID_LENGTH * GRID_HEIGHT;

	int* new_row_indexes = (int*)malloc(total_length * sizeof(int));

	if (new_row_indexes == NULL) {
		fprintf(stderr, "Mem allocation failed [get_row_indexes] (newRowIndexes)");
		exit(EXIT_MEMORY_ALLOCATE_FAILURE);
	}

	for (int i = 0; i < total_length; i++) {
		new_row_indexes[i] = -1;
	}

	for (int i = 0; i < total_length; i += GRID_LENGTH) {
		new_row_indexes[i] = i;
	}

	return new_row_indexes;
}

void calculate_win() {
	int total_grid_length = GRID_HEIGHT * GRID_LENGTH;
	int found_winner = 0;
	int total_played_rows = 0;

	/* Horizontal & vertical wins */
	for (int i = 0; i < GRID_HEIGHT; i++) {
		int horizontal_player_type = GAME_ROWS[i * GRID_LENGTH];
		int vertical_player_type = GAME_ROWS[i];

		bool horizontal_win = horizontal_player_type != player_none;
		bool vertical_win = vertical_player_type != player_none;

		for (int row_check = 1; row_check < GRID_LENGTH; row_check++) {
			if (GAME_ROWS[i * GRID_LENGTH + row_check] != horizontal_player_type) {
				horizontal_win = false;
			}
			if (GAME_ROWS[row_check * GRID_LENGTH + i] != vertical_player_type) {
				vertical_win = false;
			}
		}

		if (horizontal_win) {
			found_winner = horizontal_player_type;
			break;
		}
		if (vertical_win) {
			found_winner = vertical_player_type;
			break;
		}
	}

	/* All diag wins */
	int diagonal_player_type = GAME_ROWS[0];
	int diagonal_computer_type = GAME_ROWS[GRID_LENGTH - 1];
	bool diagonal_player_win = diagonal_player_type != player_none;
	bool diagonal_computer_win = diagonal_computer_type != player_none;

	for (int i = 1; i < GRID_LENGTH; i++) {
		if (GAME_ROWS[i * GRID_LENGTH + i] != diagonal_player_type) {
			diagonal_player_win = false;
		}
		if (GAME_ROWS[i * GRID_LENGTH + (GRID_LENGTH - 1 - i)] != diagonal_computer_type) {
			diagonal_computer_win = false;
		}
	}

	if (diagonal_player_win) {
		found_winner = diagonal_player_type;
	}
	else if (diagonal_computer_win) {
		found_winner = diagonal_computer_type;
	}

	/* Draw check */
	for (int i = 0; i < total_grid_length; i++) {
		if (GAME_ROWS[i] != 0) {
			total_played_rows++;
		}
	}

	if (found_winner != 0) {

		if (found_winner == PLAYER_GAME_TYPE) {
			TOTAL_PLAYER_WON_GAMES++;
		}
		else {
			TOTAL_COMPUTER_WON_GAMES++;
		}

		printf(found_winner == PLAYER_GAME_TYPE ? "You have won the game..." : "The computer has won the game...");
		wait_seconds(MAIN_TIMEOUT_SECONDS);
		resync_game_rows();
		choice_menu();
		return;
	}

	if (total_played_rows == total_grid_length) {
		printf("You drew with the computer! Restarting game...");
		wait_seconds(MAIN_TIMEOUT_SECONDS);
		resync_game_rows();
		choice_menu();
		return;
	}
}

void print_game_rows() {
	printf("\n [ You are playing on side %s. ]", PLAYER_TYPE_NAMES[PLAYER_GAME_TYPE]);
	printf("\n [ Computer is playing on side %s. ] \n\n", PLAYER_TYPE_NAMES[COMPUTER_GAME_TYPE]);

	int* new_row_indexes = get_row_indexes();

	printf("\t   ");

	for (int i = 0; i < GRID_LENGTH; i++) {
		printf("%i ", i);
	}

	int coord_y_counter = 0;

	for (int i = 0; i < GRID_HEIGHT * GRID_LENGTH; i++) {
		char message[TEMP_FIXED_BUFFER_FOR_STRING] = "";

		if (new_row_indexes[i] != -1) {
			snprintf(message, 50, "\n\t%d |", coord_y_counter);
			coord_y_counter++;
		}

		switch (GAME_ROWS[i])
		{
		case player_crosses: {
			strcat_s(message, TEMP_FIXED_BUFFER_FOR_STRING, "X");
			break;
		}
		case player_noughts: {
			strcat_s(message, TEMP_FIXED_BUFFER_FOR_STRING, "O");
			break;
		}
		default:
			strcat_s(message, TEMP_FIXED_BUFFER_FOR_STRING, "-");
			break;
		}

		strcat_s(message, TEMP_FIXED_BUFFER_FOR_STRING, "|");

		printf(message);
	}

	printf("\n");
	printf("\n");

	free(new_row_indexes);
}

bool set_play_on_coords(int x, int y, int game_type) {
	int index = y * GRID_HEIGHT + x;

	if (GAME_ROWS[index] != 0) {
		return false;
	}

	GAME_ROWS[index] = game_type;

	reprint_game_to_screen();

	return true;
}

void computer_take_one_turn() {
	int total_grid_length = GRID_HEIGHT * GRID_LENGTH;

	int random_index = generate_random_number(total_grid_length);

	int total_played_rows = 0;

	if (GAME_ROWS[random_index] == 0) {
		GAME_ROWS[random_index] = COMPUTER_GAME_TYPE;
	}
	else {
		computer_take_one_turn();
	}

	printf("Computer thinking...");

	wait_seconds(MAIN_TIMEOUT_SECONDS);

	calculate_win();
	reprint_game_to_screen();
	take_player_game_input();
}

void take_player_game_input() {
	int coord_y_input = 0;
	int coord_x_input = 0;

	printf("Enter Coord Y...");
	scanf_s("%i", &coord_y_input);

	if (coord_y_input > GRID_HEIGHT - 1 || coord_y_input < 0) {
		printf("Enter a valid X coord between 0 and %d.\n", GRID_HEIGHT - 1);
		wait_seconds(MAIN_TIMEOUT_SECONDS);
		take_player_game_input();
		return;
	}

	printf("Enter Coord X...");
	scanf_s("%i", &coord_x_input);

	if (coord_x_input > GRID_LENGTH - 1 || coord_x_input < 0) {
		printf("Enter a valid X coord between 0 and %d.\n", GRID_LENGTH - 1);
		wait_seconds(MAIN_TIMEOUT_SECONDS);
		take_player_game_input();
		return;
	}

	if (!set_play_on_coords(coord_x_input, coord_y_input, PLAYER_GAME_TYPE)) {
		printf("This coord is already occupied!\n");
		wait_seconds(MAIN_TIMEOUT_SECONDS);
		take_player_game_input();
		return;
	}

	calculate_win();
	computer_take_one_turn();
}

void init_game() {
	srand(time(NULL));

	resync_game_rows();
	choice_menu();
}