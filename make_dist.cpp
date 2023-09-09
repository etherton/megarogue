#include <stdio.h>
#include <math.h>

int main(int,char**) {
	printf("const uint8_t sqrt_tab[] = {\n");
	for (int i=0; i<=49+49+49; i++)
		printf("%u,",(int)(sqrtf(i) * 16.0f + 0.5f));
	printf("\n");
}
