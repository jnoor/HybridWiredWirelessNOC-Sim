#include <stdio.h>

int main() {
	
	int router_id = 0;
	int core_id = 0;
	
	for (int i = 0; i < 256 ; i++) {
		for (int k=0; k<4; k++) {
			printf("R%d: S0, C%d\n", router_id, core_id);
			core_id++;
		}
		router_id++;
	}
	
	return 0;
}
