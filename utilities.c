#pragma region Libaries
#include <time.h>
#include <stdlib.h>
#pragma endregion

int generate_random_number(int upper_bound) {
	return rand() % upper_bound;
}

void wait_seconds(int seconds) {
	clock_t start_time = clock();
	clock_t end_time = seconds * CLOCKS_PER_SEC;

	while (clock() < start_time + end_time);
}

void flush_screen() {
	system("@cls||clear");
}