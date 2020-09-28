#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	// nemu_assert(0);
	long long ans = (long long)a * b;
	return ans >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* Dividing two 64-bit integers needs the support of another library
	 * `libgcc', other than newlib. It is a dirty work to port `libgcc'
	 * to NEMU. In fact, it is unnecessary to perform a "64/64" division
	 * here. A "64/32" division is enough.
	 *
	 * To perform a "64/32" division, you can use the x86 instruction
	 * `div' or `idiv' by inline assembly. We provide a template for you
	 * to prevent you from uncessary details.
	 *
	 *     asm volatile ("??? %2" : "=a"(???), "=d"(???) : "r"(???), "a"(???), "d"(???));
	 *
	 * If you want to use the template above, you should fill the "???"
	 * correctly. For more information, please read the i386 manual for
	 * division instructions, and search the Internet about "inline assembly".
	 * It is OK not to use the template above, but you should figure
	 * out another way to perform the division.
	 */

	// nemu_assert(0);
	int s = 1, i = 0, ans;
	if(a < 0) a = -a, s = -s;
	if(b < 0) b = -b, s = -s;
	ans = a / b;
	a = a % b;
	// a is res, can calculate the low 16 bits
	for(; i < 16; ++ i){
		a <<= 1, ans <<= 1;
		if(a >= b) ++ ans, a -= b;
	}
	return s*ans;
}

FLOAT f2F(float a) {
	/* You should figure out how to convert `a' into FLOAT without
	 * introducing x87 floating point instructions. Else you can
	 * not run this code in NEMU before implementing x87 floating
	 * point instructions, which is contrary to our expectation.
	 *
	 * Hint: The bit representation of `a' is already on the
	 * stack. How do you retrieve it to another variable without
	 * performing arithmetic operations on it directly?
	 */
	// nemu_assert(0);
	// raw float
	// print_asm("f2F begin");
	printf("eip: 0x%08x f2F begin\n", cpu.eip);
	int inta = *( (int *)&a );
	int s = inta >> 31;
	int exp = (inta >> 23) & 0xff;
	int res = inta & 0x007fffff;
	if(s) s = -1; else s = 1;
	if(exp == 0) return 0;
	if(exp == 0xff) return s*0x7fffffff;
	exp -= 134; // bias
	res |= 1<<23; // the '1.'
	if(exp > 0) res <<= exp;
	if(exp < 0) res >>= -exp;
	printf("eip: 0x%08x f2F get 0x%x\n", cpu.eip, s*res);
	return s*res;
}

FLOAT Fabs(FLOAT a) {
	// nemu_assert(0);
	return a < 0 ? -a: a;
}

/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

