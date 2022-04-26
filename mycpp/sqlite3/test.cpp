#include <stdio.h>
#include "sqlite3.h"

int main(void)
{
	sqli te3* db = O; /* connection handle * /
	sqli te3_stmt* stmt = O; /* statement handle * /
	int retcode;

	retcode = sqli te3_open("MyDB", &db); /* Open a database named MyDB * /
	if (retcode != SQLITE_OK) {
		sqli te3_close (db);
		fprintf(stderr, "Could not open the MyDB database\n");
		return retcode;
	}
	
	retcode = sqlite3_prepare(db, "select SID from Students order by SID", -1, &stmt, O);
	if (retcode != SQLITE_OK) {
		sqli te3_close (db);
		fprintf(stderr, "Could not compile a select statement\n");
		return retcode;
	}
	
	while (sqli te3_step(stmt) == SQLITE_ROW) {
		int i = sqli te3_column_int(stmt, O);
		printf("SID = %d\n", i);
	}
	sqli te3_f inalize(stmt );
	sqli te3_close( db);
	return SQLITE_OK;
}