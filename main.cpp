#include "helper.h"
#include "database.h"

int main(int argc, const char **argv) {
	Database<record> db;
	db.init_groups();
	if (argc >= 2) {
		db.init_from_file(argv[1]);
		//printf("Elapsed: %.4lf\n", (double(clock()) - start) / CLOCKS_PER_SEC);
	}
	else {
		//printf("usage: ./f a.txt\n");
	}
	/*int start = clock();
	db.init_from_file("F:/bgch_files/a1000000.txt");
	printf("Elapsed: %.4lf\n", (double(clock()) - start) / CLOCKS_PER_SEC);*/
	db.run_shell();
	return 0;
}