#include "util.h"

int main() {
	int* a = (int *) xmalloc(12);
	a[1] = 12;
	free(a);
}
