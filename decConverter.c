#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	long int binary = 0;
	long int remainder = 0;
	long int hex = 0;
	int i = 1;
	
	scanf("%ld", &binary);
	while (binary != 0) {
		remainder = binary % 10;
		hex = hex + remainder * i;
		
		i = i * 2;
		binary = binary / 10;
	}
	
	
	printf("%lx", hex);
	
}

	
	
