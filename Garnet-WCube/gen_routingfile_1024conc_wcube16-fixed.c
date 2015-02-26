#include <stdio.h>
#include <stdlib.h>

int main() {
	
	int direction = 0;
	int distance = 0;
	int distance_rf = 0;
	int src_row, src_col, dst_row, dst_col;
	int meshdim = 16;
	int src_row_sec, src_col_sec, dst_row_sec, dst_col_sec;
	int src_wcube0, dst_wcube0;
	
	for (src_row = 0; src_row < meshdim +1; src_row++) {
		for (src_col = 0; src_col < meshdim ; src_col++) {
			for (dst_row = 0; dst_row < meshdim+1 ; dst_row++) {
				for (dst_col = 0; dst_col < meshdim ; dst_col++) {
					
					src_row_sec = (int)(src_row/4);
					src_col_sec = (int)(src_col/4);
					dst_row_sec = (int)(dst_row/4);
					dst_col_sec = (int)(dst_col/4);
										
					if (src_row_sec == 0) {
						if (src_col_sec == 0) {
							src_wcube0 = 0;
						} else if (src_col_sec == 1) {
							src_wcube0 = 1;
						} else if (src_col_sec == 2) {
							src_wcube0 = 4;
						} else if (src_col_sec == 3) {
							src_wcube0 = 5;
						}
					} else if (src_row_sec == 1) {
						if (src_col_sec == 0) {
							src_wcube0 = 2;
						} else if (src_col_sec == 1) {
							src_wcube0 = 3;
						} else if (src_col_sec == 2) {
							src_wcube0 = 6;
						} else if (src_col_sec == 3) {
							src_wcube0 = 7;
						}
					} else if (src_row_sec == 2) {
						if (src_col_sec == 0) {
							src_wcube0 = 8;
						} else if (src_col_sec == 1) {
							src_wcube0 = 9;
						} else if (src_col_sec == 2) {
							src_wcube0 = 12;
						} else if (src_col_sec == 3) {
							src_wcube0 = 13;
						}
					} else if (src_row_sec == 3) {
						if (src_col_sec == 0) {
							src_wcube0 = 10;
						} else if (src_col_sec == 1) {
							src_wcube0 = 11;
						} else if (src_col_sec == 2) {
							src_wcube0 = 14;
						} else if (src_col_sec == 3) {
							src_wcube0 = 15;
						}
					}
					
					if (dst_row_sec == 0) {
						if (dst_col_sec == 0) {
							dst_wcube0 = 0;
						} else if (dst_col_sec == 1) {
							dst_wcube0 = 1;
						} else if (dst_col_sec == 2) {
							dst_wcube0 = 4;
						} else if (dst_col_sec == 3) {
							dst_wcube0 = 5;
						}
					} else if (dst_row_sec == 1) {
						if (dst_col_sec == 0) {
							dst_wcube0 = 2;
						} else if (dst_col_sec == 1) {
							dst_wcube0 = 3;
						} else if (dst_col_sec == 2) {
							dst_wcube0 = 6;
						} else if (dst_col_sec == 3) {
							dst_wcube0 = 7;
						}
					} else if (dst_row_sec == 2) {
						if (dst_col_sec == 0) {
							dst_wcube0 = 8;
						} else if (dst_col_sec == 1) {
							dst_wcube0 = 9;
						} else if (dst_col_sec == 2) {
							dst_wcube0 = 12;
						} else if (dst_col_sec == 3) {
							dst_wcube0 = 13;
						}
					} else if (dst_row_sec == 3) {
						if (dst_col_sec == 0) {
							dst_wcube0 = 10;
						} else if (dst_col_sec == 1) {
							dst_wcube0 = 11;
						} else if (dst_col_sec == 2) {
							dst_wcube0 = 14;
						} else if (dst_col_sec == 3) {
							dst_wcube0 = 15;
						}
					}
					
						
					if (src_row == dst_row && src_col == dst_col) { // toward me
						direction = -6;
						distance = 0;
					} else if (src_row != 16) {  // for base router
						distance = abs(src_row - dst_row) + abs(src_col - dst_col); // when using wired routing
						
						distance_rf = abs(src_row - dst_row) + abs(src_col - dst_col);
						
						if (src_wcube0 == dst_wcube0  && dst_row != 16) { // using wired routing
							if (src_row > dst_row) {
								direction = -1;
							} else if (src_row < dst_row) {
								direction = -2;
							} else if (src_row == dst_row && src_col < dst_col) {
								direction = -3;
							} else if (src_row == dst_row && src_col > dst_col) {
								direction = -4;
							}	
						} else { // need to use wcube
							if (src_row == 0 || src_row == 4 || src_row == 8 || src_row == 12) {
								direction = -2;
							} else if (src_row == 3 || src_row == 7 || src_row == 11 || src_row == 15) {
								direction = -1;
							} else if (src_col == 0 || src_col == 4 || src_col == 8 || src_col == 12) {
								direction = -3;
							} else if (src_col == 3 || src_col == 7 || src_col == 11 || src_col == 15) {
								direction = -4;
							} else {
								direction = 0;
							}
							
						}
						
					} else if (src_row == 16 && dst_row != 16) { // from RF routers to base routers
						if (src_col == dst_wcube0) { // toward my area
							if ( (dst_row == 0 || dst_row == 1 || dst_row == 4 || dst_row == 5 || dst_row == 8 || dst_row == 9 || dst_row == 12 || dst_row == 13) && (dst_col == 0 || dst_col == 1 || dst_col == 4 || dst_col == 5 || dst_col == 8 || dst_col == 9 || dst_col == 12 || dst_col == 13) ) {
								direction = 1; //NW
							} else if ( (dst_row == 0 || dst_row == 1 || dst_row == 4 || dst_row == 5 || dst_row == 8 || dst_row == 9 || dst_row == 12 || dst_row == 13) && (dst_col == 2 || dst_col == 3 || dst_col == 6 || dst_col == 7 || dst_col == 10 || dst_col == 11 || dst_col == 14 || dst_col == 15) ) {
								direction = 2; //NE
							} else if ( (dst_row == 2 || dst_row == 3 || dst_row == 6 || dst_row == 7 || dst_row == 10 || dst_row == 11 || dst_row == 14 || dst_row == 15) && (dst_col == 0 || dst_col == 1 || dst_col == 4 || dst_col == 5 || dst_col == 8 || dst_col == 9 || dst_col == 12 || dst_col == 13) ) {
								direction = 3; //SW
							} else if ( (dst_row == 2 || dst_row == 3 || dst_row == 6 || dst_row == 7 || dst_row == 10 || dst_row == 11 || dst_row == 14 || dst_row == 15) && (dst_col == 2 || dst_col == 3 || dst_col == 6 || dst_col == 7 || dst_col == 10 || dst_col == 11 || dst_col == 14 || dst_col == 15) ) {
								direction = 4; //SE
							}else {
								printf("ERROR1\n");
							}
						} else { // need to traverse wcube
							if (src_col == 0) {
								if (dst_wcube0 == 0) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 1) {
									direction = 9;
								} else if (dst_wcube0 == 2) {
									direction = 8;
								} else if (dst_wcube0 == 3) {
									direction = 8;
								} else if (dst_wcube0 == 4) {
									direction = 7;
								} else if (dst_wcube0 == 5) {
									direction = 7;
								} else if (dst_wcube0 == 6) {
									direction = 7;
								} else if (dst_wcube0 == 7) {
									direction = 7;
								} else if (dst_wcube0 == 8) {
									direction = 6;
								} else if (dst_wcube0 == 9) {
									direction = 6;
								} else if (dst_wcube0 == 10) {
									direction = 6;
								} else if (dst_wcube0 == 11) {
									direction = 6;
								} else if (dst_wcube0 == 12) {
									direction = 6;
								} else if (dst_wcube0 == 13) {
									direction = 6;
								} else if (dst_wcube0 == 14) {
									direction = 6;
								} else if (dst_wcube0 == 15) {
									direction = 6;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 1) {
								if (dst_wcube0 == 0) {
									direction = 9;
								} else if (dst_wcube0 == 1) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 2) {
									direction = 8;
								} else if (dst_wcube0 == 3) {
									direction = 8;
								} else if (dst_wcube0 == 4) {
									direction = 7;
								} else if (dst_wcube0 == 5) {
									direction = 7;
								} else if (dst_wcube0 == 6) {
									direction = 7;
								} else if (dst_wcube0 == 7) {
									direction = 7;
								} else if (dst_wcube0 == 8) {
									direction = 6;
								} else if (dst_wcube0 == 9) {
									direction = 6;
								} else if (dst_wcube0 == 10) {
									direction = 6;
								} else if (dst_wcube0 == 11) {
									direction = 6;
								} else if (dst_wcube0 == 12) {
									direction = 6;
								} else if (dst_wcube0 == 13) {
									direction = 6;
								} else if (dst_wcube0 == 14) {
									direction = 6;
								} else if (dst_wcube0 == 15) {
									direction = 6;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 2) {
								if (dst_wcube0 == 0) {
									direction = 8;
								} else if (dst_wcube0 == 1) {
									direction = 8;
								} else if (dst_wcube0 == 2) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 3) {
									direction = 9;
								} else if (dst_wcube0 == 4) {
									direction = 7;
								} else if (dst_wcube0 == 5) {
									direction = 7;
								} else if (dst_wcube0 == 6) {
									direction = 7;
								} else if (dst_wcube0 == 7) {
									direction = 7;
								} else if (dst_wcube0 == 8) {
									direction = 6;
								} else if (dst_wcube0 == 9) {
									direction = 6;
								} else if (dst_wcube0 == 10) {
									direction = 6;
								} else if (dst_wcube0 == 11) {
									direction = 6;
								} else if (dst_wcube0 == 12) {
									direction = 6;
								} else if (dst_wcube0 == 13) {
									direction = 6;
								} else if (dst_wcube0 == 14) {
									direction = 6;
								} else if (dst_wcube0 == 15) {
									direction = 6;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 3) {
								if (dst_wcube0 == 0) {
									direction = 8;
								} else if (dst_wcube0 == 1) {
									direction = 8;
								} else if (dst_wcube0 == 2) {
									direction = 9;
								} else if (dst_wcube0 == 3) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 4) {
									direction = 7;
								} else if (dst_wcube0 == 5) {
									direction = 7;
								} else if (dst_wcube0 == 6) {
									direction = 7;
								} else if (dst_wcube0 == 7) {
									direction = 7;
								} else if (dst_wcube0 == 8) {
									direction = 6;
								} else if (dst_wcube0 == 9) {
									direction = 6;
								} else if (dst_wcube0 == 10) {
									direction = 6;
								} else if (dst_wcube0 == 11) {
									direction = 6;
								} else if (dst_wcube0 == 12) {
									direction = 6;
								} else if (dst_wcube0 == 13) {
									direction = 6;
								} else if (dst_wcube0 == 14) {
									direction = 6;
								} else if (dst_wcube0 == 15) {
									direction = 6;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 4) {
								if (dst_wcube0 == 0) {
									direction = 7;
								} else if (dst_wcube0 == 1) {
									direction = 7;
								} else if (dst_wcube0 == 2) {
									direction = 7;
								} else if (dst_wcube0 == 3) {
									direction = 7;
								} else if (dst_wcube0 == 4) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 5) {
									direction = 9;
								} else if (dst_wcube0 == 6) {
									direction = 8;
								} else if (dst_wcube0 == 7) {
									direction = 8;
								} else if (dst_wcube0 == 8) {
									direction = 6;
								} else if (dst_wcube0 == 9) {
									direction = 6;
								} else if (dst_wcube0 == 10) {
									direction = 6;
								} else if (dst_wcube0 == 11) {
									direction = 6;
								} else if (dst_wcube0 == 12) {
									direction = 6;
								} else if (dst_wcube0 == 13) {
									direction = 6;
								} else if (dst_wcube0 == 14) {
									direction = 6;
								} else if (dst_wcube0 == 15) {
									direction = 6;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 5) {
								if (dst_wcube0 == 0) {
									direction = 7;
								} else if (dst_wcube0 == 1) {
									direction = 7;
								} else if (dst_wcube0 == 2) {
									direction = 7;
								} else if (dst_wcube0 == 3) {
									direction = 7;
								} else if (dst_wcube0 == 4) {
									direction = 9;
								} else if (dst_wcube0 == 5) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 6) {
									direction = 8;
								} else if (dst_wcube0 == 7) {
									direction = 8;
								} else if (dst_wcube0 == 8) {
									direction = 6;
								} else if (dst_wcube0 == 9) {
									direction = 6;
								} else if (dst_wcube0 == 10) {
									direction = 6;
								} else if (dst_wcube0 == 11) {
									direction = 6;
								} else if (dst_wcube0 == 12) {
									direction = 6;
								} else if (dst_wcube0 == 13) {
									direction = 6;
								} else if (dst_wcube0 == 14) {
									direction = 6;
								} else if (dst_wcube0 == 15) {
									direction = 6;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 6) {
								if (dst_wcube0 == 0) {
									direction = 7;
								} else if (dst_wcube0 == 1) {
									direction = 7;
								} else if (dst_wcube0 == 2) {
									direction = 7;
								} else if (dst_wcube0 == 3) {
									direction = 7;
								} else if (dst_wcube0 == 4) {
									direction = 8;
								} else if (dst_wcube0 == 5) {
									direction = 8;
								} else if (dst_wcube0 == 6) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 7) {
									direction = 9;
								} else if (dst_wcube0 == 8) {
									direction = 6;
								} else if (dst_wcube0 == 9) {
									direction = 6;
								} else if (dst_wcube0 == 10) {
									direction = 6;
								} else if (dst_wcube0 == 11) {
									direction = 6;
								} else if (dst_wcube0 == 12) {
									direction = 6;
								} else if (dst_wcube0 == 13) {
									direction = 6;
								} else if (dst_wcube0 == 14) {
									direction = 6;
								} else if (dst_wcube0 == 15) {
									direction = 6;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 7) {
								if (dst_wcube0 == 0) {
									direction = 7;
								} else if (dst_wcube0 == 1) {
									direction = 7;
								} else if (dst_wcube0 == 2) {
									direction = 7;
								} else if (dst_wcube0 == 3) {
									direction = 7;
								} else if (dst_wcube0 == 4) {
									direction = 8;
								} else if (dst_wcube0 == 5) {
									direction = 8;
								} else if (dst_wcube0 == 6) {
									direction = 9;
								} else if (dst_wcube0 == 7) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 8) {
									direction = 6;
								} else if (dst_wcube0 == 9) {
									direction = 6;
								} else if (dst_wcube0 == 10) {
									direction = 6;
								} else if (dst_wcube0 == 11) {
									direction = 6;
								} else if (dst_wcube0 == 12) {
									direction = 6;
								} else if (dst_wcube0 == 13) {
									direction = 6;
								} else if (dst_wcube0 == 14) {
									direction = 6;
								} else if (dst_wcube0 == 15) {
									direction = 6;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 8) {
								if (dst_wcube0 == 0) {
									direction = 6;
								} else if (dst_wcube0 == 1) {
									direction = 6;
								} else if (dst_wcube0 == 2) {
									direction = 6;
								} else if (dst_wcube0 == 3) {
									direction = 6;
								} else if (dst_wcube0 == 4) {
									direction = 6;
								} else if (dst_wcube0 == 5) {
									direction = 6;
								} else if (dst_wcube0 == 6) {
									direction = 6;
								} else if (dst_wcube0 == 7) {
									direction = 6;
								} else if (dst_wcube0 == 8) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 9) {
									direction = 9;
								} else if (dst_wcube0 == 10) {
									direction = 8;
								} else if (dst_wcube0 == 11) {
									direction = 8;
								} else if (dst_wcube0 == 12) {
									direction = 7;
								} else if (dst_wcube0 == 13) {
									direction = 7;
								} else if (dst_wcube0 == 14) {
									direction = 7;
								} else if (dst_wcube0 == 15) {
									direction = 7;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 9) {
								if (dst_wcube0 == 0) {
									direction = 6;
								} else if (dst_wcube0 == 1) {
									direction = 6;
								} else if (dst_wcube0 == 2) {
									direction = 6;
								} else if (dst_wcube0 == 3) {
									direction = 6;
								} else if (dst_wcube0 == 4) {
									direction = 6;
								} else if (dst_wcube0 == 5) {
									direction = 6;
								} else if (dst_wcube0 == 6) {
									direction = 6;
								} else if (dst_wcube0 == 7) {
									direction = 6;
								} else if (dst_wcube0 == 8) {
									direction = 9;
								} else if (dst_wcube0 == 9) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 10) {
									direction = 8;
								} else if (dst_wcube0 == 11) {
									direction = 8;
								} else if (dst_wcube0 == 12) {
									direction = 7;
								} else if (dst_wcube0 == 13) {
									direction = 7;
								} else if (dst_wcube0 == 14) {
									direction = 7;
								} else if (dst_wcube0 == 15) {
									direction = 7;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 10) {
								if (dst_wcube0 == 0) {
									direction = 6;
								} else if (dst_wcube0 == 1) {
									direction = 6;
								} else if (dst_wcube0 == 2) {
									direction = 6;
								} else if (dst_wcube0 == 3) {
									direction = 6;
								} else if (dst_wcube0 == 4) {
									direction = 6;
								} else if (dst_wcube0 == 5) {
									direction = 6;
								} else if (dst_wcube0 == 6) {
									direction = 6;
								} else if (dst_wcube0 == 7) {
									direction = 6;
								} else if (dst_wcube0 == 8) {
									direction = 8;
								} else if (dst_wcube0 == 9) {
									direction = 8;
								} else if (dst_wcube0 == 10) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 11) {
									direction = 9;
								} else if (dst_wcube0 == 12) {
									direction = 7;
								} else if (dst_wcube0 == 13) {
									direction = 7;
								} else if (dst_wcube0 == 14) {
									direction = 7;
								} else if (dst_wcube0 == 15) {
									direction = 7;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 11) {
								if (dst_wcube0 == 0) {
									direction = 6;
								} else if (dst_wcube0 == 1) {
									direction = 6;
								} else if (dst_wcube0 == 2) {
									direction = 6;
								} else if (dst_wcube0 == 3) {
									direction = 6;
								} else if (dst_wcube0 == 4) {
									direction = 6;
								} else if (dst_wcube0 == 5) {
									direction = 6;
								} else if (dst_wcube0 == 6) {
									direction = 6;
								} else if (dst_wcube0 == 7) {
									direction = 6;
								} else if (dst_wcube0 == 8) {
									direction = 8;
								} else if (dst_wcube0 == 9) {
									direction = 8;
								} else if (dst_wcube0 == 10) {
									direction = 9;
								} else if (dst_wcube0 == 11) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 12) {
									direction = 7;
								} else if (dst_wcube0 == 13) {
									direction = 7;
								} else if (dst_wcube0 == 14) {
									direction = 7;
								} else if (dst_wcube0 == 15) {
									direction = 7;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 12) {
								if (dst_wcube0 == 0) {
									direction = 6;
								} else if (dst_wcube0 == 1) {
									direction = 6;
								} else if (dst_wcube0 == 2) {
									direction = 6;
								} else if (dst_wcube0 == 3) {
									direction = 6;
								} else if (dst_wcube0 == 4) {
									direction = 6;
								} else if (dst_wcube0 == 5) {
									direction = 6;
								} else if (dst_wcube0 == 6) {
									direction = 6;
								} else if (dst_wcube0 == 7) {
									direction = 6;
								} else if (dst_wcube0 == 8) {
									direction = 7;
								} else if (dst_wcube0 == 9) {
									direction = 7;
								} else if (dst_wcube0 == 10) {
									direction = 7;
								} else if (dst_wcube0 == 11) {
									direction = 7;
								} else if (dst_wcube0 == 12) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 13) {
									direction = 9;
								} else if (dst_wcube0 == 14) {
									direction = 8;
								} else if (dst_wcube0 == 15) {
									direction = 8;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 13) {
								if (dst_wcube0 == 0) {
									direction = 6;
								} else if (dst_wcube0 == 1) {
									direction = 6;
								} else if (dst_wcube0 == 2) {
									direction = 6;
								} else if (dst_wcube0 == 3) {
									direction = 6;
								} else if (dst_wcube0 == 4) {
									direction = 6;
								} else if (dst_wcube0 == 5) {
									direction = 6;
								} else if (dst_wcube0 == 6) {
									direction = 6;
								} else if (dst_wcube0 == 7) {
									direction = 6;
								} else if (dst_wcube0 == 8) {
									direction = 7;
								} else if (dst_wcube0 == 9) {
									direction = 7;
								} else if (dst_wcube0 == 10) {
									direction = 7;
								} else if (dst_wcube0 == 11) {
									direction = 7;
								} else if (dst_wcube0 == 12) {
									direction = 9;
								} else if (dst_wcube0 == 13) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 14) {
									direction = 8;
								} else if (dst_wcube0 == 15) {
									direction = 8;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 14) {
								if (dst_wcube0 == 0) {
									direction = 6;
								} else if (dst_wcube0 == 1) {
									direction = 6;
								} else if (dst_wcube0 == 2) {
									direction = 6;
								} else if (dst_wcube0 == 3) {
									direction = 6;
								} else if (dst_wcube0 == 4) {
									direction = 6;
								} else if (dst_wcube0 == 5) {
									direction = 6;
								} else if (dst_wcube0 == 6) {
									direction = 6;
								} else if (dst_wcube0 == 7) {
									direction = 6;
								} else if (dst_wcube0 == 8) {
									direction = 7;
								} else if (dst_wcube0 == 9) {
									direction = 7;
								} else if (dst_wcube0 == 10) {
									direction = 7;
								} else if (dst_wcube0 == 11) {
									direction = 7;
								} else if (dst_wcube0 == 12) {
									direction = 8;
								} else if (dst_wcube0 == 13) {
									direction = 8;
								} else if (dst_wcube0 == 14) {
									printf("ERROR2\n");
								} else if (dst_wcube0 == 15) {
									direction = 9;
								} else {
									printf("ERROR3\n");
								}
							} else if (src_col == 15) {
								if (dst_wcube0 == 0) {
									direction = 6;
								} else if (dst_wcube0 == 1) {
									direction = 6;
								} else if (dst_wcube0 == 2) {
									direction = 6;
								} else if (dst_wcube0 == 3) {
									direction = 6;
								} else if (dst_wcube0 == 4) {
									direction = 6;
								} else if (dst_wcube0 == 5) {
									direction = 6;
								} else if (dst_wcube0 == 6) {
									direction = 6;
								} else if (dst_wcube0 == 7) {
									direction = 6;
								} else if (dst_wcube0 == 8) {
									direction = 7;
								} else if (dst_wcube0 == 9) {
									direction = 7;
								} else if (dst_wcube0 == 10) {
									direction = 7;
								} else if (dst_wcube0 == 11) {
									direction = 7;
								} else if (dst_wcube0 == 12) {
									direction = 8;
								} else if (dst_wcube0 == 13) {
									direction = 8;
								} else if (dst_wcube0 == 14) {
									direction = 9;
								} else if (dst_wcube0 == 15) {
									printf("ERROR2\n");
								} else {
									printf("ERROR3\n");
								}
							}
						}
						
					} else if (src_row == 16 && dst_row == 16) { // for RF routers
						direction = 0; //dummy!!!
					}
					
					printf("%d %d \n", direction, distance);
						
				  }
				}
			}
	}
	
	return 0;
}
