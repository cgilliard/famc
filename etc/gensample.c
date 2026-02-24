#include <stdio.h>

#define SIZE 10000

int main(int argc, char **argv) {
	int i;
	FILE *fp;
	fp = fopen(argv[1], "w");
	fprintf(fp, "int main(int argc, char **argv) {\n");
	fprintf(fp, "\tint x0 = argc;\n");
	for (i = 1; i < SIZE; i++) {
		fprintf(fp, "\tint x%d = %d;\n", i, i + 3);
	}
	fprintf(fp, "\treturn (");
	for (i = 0; i < SIZE; i++) {
		if (i + 1 != SIZE)
			fprintf(fp, "x%d + ", i);
		else
			fprintf(fp, "%d", i);
	}
	fprintf(fp, ");\n}");
	fclose(fp);
	return 0;
}
