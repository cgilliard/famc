void proc_struct_complete(struct parser *p) {
	{ long x = 4; }
	{ long y = 3; }

	{
		long z = 2;
		{ long v = 1; }
	}
}

void proc2(struct x y) {
	{
		char ch;
		long x;
		x = 0;
	}
	{
		long v;
		{ char ch2; }
	}
}

