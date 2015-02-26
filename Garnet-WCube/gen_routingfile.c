#include <stdio.h>
#include <stdlib.h>

int main() {
	
	int direction = 0;
	int distance = 0;
	int src_row, src_col, dst_row, dst_col;
	int meshdim = 16;
	
	for (src_row = 0; src_row < meshdim ; src_row++) {
		for (src_col = 0; src_col < meshdim ; src_col++) {
			for (dst_row = 0; dst_row < meshdim ; dst_row++) {
				for (dst_col = 0; dst_col < meshdim ; dst_col++) {
						
					if (src_row == dst_row && src_col == dst_col) {
						direction = -6;
						distance = 0;
					} else {
						distance = abs(src_row - dst_row) + abs(src_col - dst_col);
						
						if (src_row > dst_row) {
							direction = -1;
						} else if (src_row < dst_row) {
							direction = -2;
						} else if (src_row == dst_row && src_col < dst_col) {
							direction = -3;
						} else if (src_row == dst_row && src_col > dst_col) {
							direction = -4;
						}
						
					}
					
					printf("%d %d \n", direction, distance);
						
					}
				}
			}
	}
	
	return 0;
}
