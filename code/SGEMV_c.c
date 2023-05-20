#include<stdio.h>
#include <stdlib.h>

#define true 1;
#define false 0;

void print_MAT(float* Mat, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printf("%5.2f | ", Mat[i * cols + j]);
    }
    printf("\n");
    for (int k = 0; k < cols; k++) {
      printf("--------");
    }
    printf("\n");
  }
}

void print_VECT(float* Vect, int len, int inc){
  for (int j = 0; j < len; j++) {
      printf("%5.2f | ", Vect[j*inc]);
  }
  printf("\n");
}

void SGEMV(char TRANS, int M, int N, float ALPHA, float* A, int LDA, float* X, int INCX, float BETA, float*  Y, int INCY){

    int TR;
    
    // Verification of parameters
    if (TRANS == 'T' || TRANS == 't' || TRANS == 'C' || TRANS == 'c' ){
        TR = true;
    }else{
    if (TRANS == 'N' || TRANS == 'n'){
        TR = false;
    }else{
        printf("Invalid value of TRANS !\n");
        exit(-1);
    }
    }

    if (LDA < M){
    printf("Invalid Value of LDA !\n");
    exit(-1);
    }
    float A_TMP[N][M];

    // Appliquer la transposée
    if (TR){
        for (int i=0; i<M; i++){
            for(int j=0; j<N; j++){
                A_TMP[j][i] = A[i * N+ j];
            }
        }
        printf("A (after transposition) : \n----- \n");
        print_MAT(&A_TMP[0][0], N, M);
        printf("\n");
    }

    //
    if (TR){
        // Calculate ALPHA*A**T*X + BETA*Y
        for(int i = 0; i< N; i++){
            Y[i * INCY] *= BETA;
            for(int j=0; j < M; j++){
               Y[i * INCY] += ALPHA * A_TMP[i][j] * X[j*INCX]; 
            } 
        }

    }else{
        // Calculate ALPHA*A*X + BETA*Y 
        for(int i = 0; i< M; i++){
            Y[i * INCY] *= BETA;
            for(int j=0; j < N; j++){
               Y[i * INCY] += ALPHA * A[i * N + j] * X[j*INCX]; 
            } 
        }
    }
    
}


void main(){
    char trans = 'C';
    int incx = 2, incy = 3;
    int i,j;

    // Transposition of matrix 
    int n = 5;
    int m = 4;

    float A[4][5]= {{1.0f, 0.0f, 0.0f, 0.0f,1.0f},
                    {2.0f, 3.0f, 0.0f, 0.0f, 5.0f},
                    {4.0f, 5.0f, 6.0f, 0.0f,10.f},
                    {7.0f, 8.0f, 9.0f, 10.0f, 25.0f}};    

    printf("A : \n----- \n");
    print_MAT(&A[0][0] , m,n);
    printf("\n");

    float alpha = 2.5 , beta = -1.2 ;
    float X[4 * 2] = {1.0, 0.0 , 2.0, 1.2 , 4.0, 12.0 , 5.0, 0.0};
    float Y[5 * 3] = {6.0 , 1.0, 2.0 , 0.0, 1.0, 2.0 , 1.0, 1.0, 2.0 , 3.0, 1.0, 2.0, 3.0, 1.0, 2.0};


    printf("X : \n----- \n");
    print_VECT(X , m, incx);
    printf("\n");
    
    printf("Y (before) : \n----- \n");
    print_VECT(Y , n, incy);
    printf("\n");
  
    SGEMV(trans, m, n, alpha, &A[0][0], m, X, incx, beta, Y, incy);

    printf("Y (after) : \n----- \n");
    print_VECT(Y , n, incy);

    //printing the result in a file
    FILE *f;

    f = fopen("../results/c_result.txt", "w+");

    for (int i = 0; i < n; i++)
    {
        fprintf(f, "%5.2f ", Y[i*incy]);
    }

    fclose(f);
}


