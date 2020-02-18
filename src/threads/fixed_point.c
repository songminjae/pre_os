#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>

#define F (1 << 14)
#define INT_MAX ((1 << 31) - 1)
#define INT_MIN (-(1 << 31))

#define FRACTION (1<<14)

int int_to_fp(int n); /* integer를 fixed point로 전환 */ 
int fp_to_int_round(int x); /* FP를 int로 전환(반올림) */ 
int fp_to_int(int x); /* FP를 int로 전환(버림) */ 
int add_fp(int x, int y); /* FP의 덧셈 */ 
int add_mixed(int x, int n); /* FP와 int의 덧셈 */ 
int sub_fp(int x, int y); /* FP의 뺄셈(x-y) */ 
int sub_mixed(int x, int n); /* FP와 int의 뺄셈(x-n) */ 
int mult_fp(int x, int y); /* FP의 곱셈 */ 
int mult_mixed(int x, int y); /* FP와 int의 곱셈 */ 
int div_fp(int x, int y); /* FP의 나눗셈(x/y) */ 
int div_mixed(int x, int n); /* FP와 int 나눗셈(x/n) */
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////  n 이 정수 

int int_to_fp(int n){
	return n * F;
}

int fp_to_int_round(int x){
	return x / F;
}

int fp_to_int(int x){
	if (x >= 0){
		return (x+F/2)/F;
	}
	else{
		return (x-F/2)/F;
	}
}


///

int add_fp(int x, int y){
	return x + y;
}

int add_mixed(int x, int n){
	return x + n * F;
}

int sub_fp(int x, int y){
	return x - y;
}

int sub_mixed(int x, int n){
	return x - n * F;
}

int mult_fp(int x, int y){
	int64_t temp = x;
	temp = temp * y / F;
	return (int) temp;
}

int mult_mixed(int x, int y){
	return x * y;
}

int div_fp(int x, int y){
	int64_t temp = x;
	temp = temp * F / y;
	return (int)temp;
}

int div_mixed(int x, int n){
	return x / n;
}

/////////////////////////////////////////////////////////////

int int_sub_float(int i, int f) {
  return i*FRACTION - f;
}

int int_mul_float(int i, int f) {
  return i*f;
}

int float_add_int(int f, int i) {
  return f + i*FRACTION;
}

int float_mul_float(int f, int i) {
  // 오버플로우가 발생할 수 있으니 64bit 데이터를 중간 계산 과정에 사용
  int64_t temp = f;
  temp = temp * i / FRACTION;
  return (int)temp;
}

int float_div_float(int f1, int f2) {
  int64_t temp = f1;
  temp = temp * FRACTION / f2;
  return (int)temp;
}

int float_add_float(int f1, int f2) {
  return f1 + f2;
}

int float_sub_float(int f1, int f2) {
  return f1 - f2;
}

int float_div_int(int f, int i) {
  return f / i;
}