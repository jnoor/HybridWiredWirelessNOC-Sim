#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() {
	
	int cycle = 1000000;
	int src_coreid = 0;
	int dst_coreid = 1023;
	int packet_size = 8;
	int i,j,k,l;
	
	time_t dummy;
	srand((unsigned int) time(&dummy));
	
	for (i = 1; i < cycle ; i++) {
		l = 1;// rand() % 5;
		for (j = 0; j <= l; j++) {
			src_coreid = rand() % 1024;
			dst_coreid = rand() % 1024;
			while (src_coreid == dst_coreid) {
				dst_coreid = rand() % 1024;
			}
			
			if (j%2 == 0) {
				packet_size = 8;
			} else {
				packet_size = 32;
			}
			
			printf("C %d C %d %d 0 %d \n", src_coreid, dst_coreid, packet_size, i);
		}
	}
	
	return 0;
}
