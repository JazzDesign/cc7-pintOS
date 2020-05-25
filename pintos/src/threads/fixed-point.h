// #define P 17 //p.q
// #define Q (31-P)
#define F 16384
int32_t n_to_fp(int n);
int32_t x_to_int_T(int);
int32_t x_to_int_N(int);
int32_t add_fp(int, int);
int32_t sub_fp(int, int);
int32_t mul_x_y(int, int);
int32_t mul_x_n(int,int);
int32_t div_x_y(int,int);
int32_t div_x_n(int,int);


int32_t n_to_fp(int n){
  return (int32_t)(n*F);
}
int32_t x_to_int_T(int x){
  return (int32_t)(x/F);
}
int32_t x_to_int_N(int x){
  if (x >= 0){
    return (int32_t) ((x+(F/2))/F);
  }
  if(x <= 0){
    return (int32_t) ((x-(F/2))/F);
  }
}
int32_t add_fp(int x, int y){
  return (int32_t) (x+y);
}
int32_t sub_fp(int x, int y){
  return (int32_t) (x-y);
}
int32_t mul_x_y(int x, int y){
  return (int32_t) (((int64_t)x)*y/F);
}
int32_t mul_x_n(int x, int n){
  return (int32_t) (x*n);
}
int32_t div_x_y(int x, int y){
 return (int32_t) (((int64_t)x)*F/y);
}
int32_t div_x_n(int x, int n){
  return (int32_t)(x/n);
}
