#include "Halide.h"
#include <stdio.h>
#include <iomanip>   
#include <iostream> 
#include <fstream>
using namespace Halide;
using namespace std;
#define true 1;
#define false 0;

void print_MAT(Buffer < float > Mat, int rows, int cols) {
     cout << fixed << setprecision(2);  
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      cout << setw(5) << Mat(i, j) << " | ";
    }
    cout << "\n";
    for (int k = 0; k < cols; k++) {
      cout << "--------";
      
    }
    cout << "\n";
  }
}

void print_VECT(Buffer < float > Vect, int len, int inc){
  cout << fixed << setprecision(2);
  for (int j = 0; j < len; j++) {
      cout << setw(5) << Vect(j*inc) << " | ";
  }
  cout << "\n";
  
}

void SGEMV(char TRANS, int M, int N, float ALPHA, Buffer < float > A, int LDA, Buffer < float > X, int INCX, float BETA, Buffer < float > Y, int INCY){
  
  int TR;
  Var x, y;
  // Verification of parameters
  if (TRANS == 'T' || TRANS == 't' || TRANS == 'C' || TRANS == 'c' ){
    TR = true;
  }else{
    if (TRANS == 'N' || TRANS == 'n'){
      TR = false;
    }else{
      cout << "Invalid value of TRANS !\n";
      exit(-1);
    }
  }

  if (TR && X.dim(0).extent() < ( 1 + ( M - 1 )*abs( INCX )) || !TR && X.dim(0).extent() < ( 1 + ( N - 1 )*abs( INCX )) ){
    cout << "Vector X too small !\n";
    exit(-1);
  }

  if (TR && Y.dim(0).extent() < ( 1 + ( N - 1 )*abs( INCY )) || !TR && Y.dim(0).extent() < ( 1 + ( M - 1 )*abs( INCY )) ){
    cout << "Vector Y too small !\n";
    exit(-1);
  }

  if (LDA < M){
    cout << "Invalid Value of LDA !\n";
    exit(-1);
  }

  Buffer <float> A_TMP1(M,N);
  Buffer <float> A_TMP2(N,M);

  // The function that calculte A**T
  Func Tran;  
  Tran(x,y) = A_TMP1(y,x);
  RDom r(0, N, 0, M);
  r.where(r.x != r.y);
  Tran(r.x,r.y);


  // Extract the leading M by N element
  for (int i = 0; i < M; i++ ){
    for (int j = 0; j < N; j++ ){
      A_TMP1(i,j) = A(i,j);
    }
  }
  
  // Calculate A**T
  if (TR){
    A_TMP2 = Tran.realize({A.height(), A.width()});
    cout << "A (after transposition) : \n----- \n";
    print_MAT(A_TMP2, N, M);
    cout << "\n";
  }
  
  // The function that calculate ALPHA*A*X + BETA*Y Or ALPHA*A**T*X + BETA*Y
  Func CALC;
  RDom r1(0 , N);
  RDom r4(0 , M);
  r4.where(r4 % INCY == 0);
  CALC(x) = ALPHA * sum(A_TMP1(x/INCY,r1) * X(r1* INCX)) + BETA * Y(x*INCY);
  CALC(r4);

  // The function that calculate ALPHA*A*X + BETA*Y
  Func CALC_T;
  RDom r2(0 , M);
  RDom r3(0 , N);
  r3.where(r3 % INCY == 0);
  CALC_T(x) = ALPHA * sum(A_TMP2(x/INCY,r2) * X(r2* INCX)) + BETA * Y(x);


  CALC_T(r3);
  if (TR){
    // Calculate ALPHA*A**T*X + BETA*Y
    CALC_T.realize(Y);
  }else{
    // Calculate ALPHA*A*X + BETA*Y 
    CALC.realize(Y);
  }
  
}
 
int main(int argc, char **argv) {
  char trans = 'C';
  int incx = 2, incy = 3;
  int i,j;

    // Transposition of matrix 
    int n =5;
    int m = 4;

    Buffer<float> input(m, n) ;
    float A[m][n]= {{1.0f, 0.0f, 0.0f, 0.0f,1.0f},
                    {2.0f, 3.0f, 0.0f, 0.0f, 5.0f},
                    {4.0f, 5.0f, 6.0f, 0.0f,10.f},
                    {7.0f, 8.0f, 9.0f, 10.0f, 25.0f}};
    

    for (i=0; i< m; i++){
        for (int j=0; j < n; j++){
            input(i,j) = A[i][j];
        }
    }

    cout << "A : \n----- \n";
    print_MAT(input , m,n);
    cout << "\n";

    float alpha = 2.5 , beta = -1.2 ;
    float X[m *incx] = {1.0, 0.0 , 2.0, 1.2 , 4.0, 12.0 , 5.0, 0.0};
    float Y[n*incy] = {6.0 , 1.0, 2.0 , 0.0, 1.0, 2.0 , 1.0, 1.0, 2.0 , 3.0, 1.0, 2.0, 3.0, 1.0, 2.0};


    Buffer <float> input2(m*incx);
    Buffer <float> input3(n*incy);

    for (j=0; j < n*incy; j++){
            input3(j) = Y[j];
    }

    for (j=0; j < m*incx; j++){
            input2(j) = X[j];
    }

    cout << "X : \n----- \n";
    print_VECT(input2 , m, incx);
    cout << "\n";
    
    cout << "Y (before) : \n----- \n";
    print_VECT(input3 , n, incy);
    cout << "\n";
  
    SGEMV(trans, m, n, alpha, input, m, input2, incx, beta, input3, incy);

    cout << "Y (after) : \n----- \n";
    print_VECT(input3 , n, incy);
    


    //printing the result in a file
    ofstream f("../results/halide_result.txt");
    f << fixed << setprecision(2); 
    for (int i = 0; i < n; i++)
    {
        f << input3(i * incy) << " ";
    }
    f.close();
    return 0;
}
