#include <stdio.h>

#define SIZE 10000

int main(int argc, char **argv) {
	int i;
	FILE *fp;
	fp = fopen(argv[1], "w");
	for (i = 1; i < SIZE; i++)
		fprintf(fp, "void fun%d (int *r, int v) { *r = v * %d; }\n", i,
			i);
	fprintf(fp,
		"\nint main(int argc, char **argv) {\n\tint sum;\n\n\tsum = "
		"argc;\n");
	for (i = 1; i < SIZE; i++)
		fprintf(fp, "\tfun%d(&sum, sum); sum += argc + %d;\n", i, i);
	fprintf(fp, "\treturn sum;\n}\n");
	fclose(fp);
	return 0;
}
