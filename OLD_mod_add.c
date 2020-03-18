/*
 * mod_add.c
 *
 */

#include <stdint.h>
// Calculates res = a + b.
// a and b represent large integers stored in uint32_t arrays
// a and b are arrays of size elements, res has size+1 elements
void mp_add(uint32_t *a, uint32_t *b, uint32_t *res, uint32_t size)
{
	uint32_t c = 0;
	uint32_t i;
	uint64_t sum;

	for (i=0;i<size;i++) {
		sum = (uint64_t) a[i] + (uint64_t) b[i] + (uint64_t) c;
		res[i] = sum;
		c = sum>>32;

	}
	res[32] = c;
}

// Calculates res = a - b.
// a and b represent large integers stored in uint32_t arrays
// a, b and res are arrays of size elements
void mp_sub(uint32_t *a, uint32_t *b, uint32_t *res, uint32_t size)
{
		uint32_t c = 0;
		uint32_t i;

		for (i=0;i<size;i++) {
			res[i] = a[i] - b[i] - c;
			if (a[i] < b[i] + c) {
				c = 1;
			} else {
				c = 0;
			}
		}
}

// Calculates res = (a + b) mod N.
// a and b represent operands, N is the modulus. They are large integers stored in uint32_t arrays of size elements
void mod_add(uint32_t *a, uint32_t *b, uint32_t *N, uint32_t *res, uint32_t size)
{
	uint32_t z = 31;
	uint32_t sum[33];
	uint32_t i;

	mp_add(a, b, sum, size);
	if (sum[32] == 1) {
		mp_sub(sum, N, res, size);
	} 
	else {
		while (sum[z] == N[z]) {
				z--;
			}
		if (sum[z] > N[z]) {
			mp_sub(sum, N, res, size);
		}
		else {
			for (i = 0; i<size; i++){
				res[i] = sum[i];
			}
		}
	}


}


