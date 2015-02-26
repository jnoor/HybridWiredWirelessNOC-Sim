#include <stdio.h>

int main() {
	
	int router_id = 0;
	int core_id = 0;
	int i,k;
	
	for (i = 0; i < 256 ; i++) {
		for (k=0; k<4; k++) {
			printf("R%d: S0, C%d\n", router_id, core_id);
			core_id++;
		}
		router_id++;
	}
	
	return 0;
}
