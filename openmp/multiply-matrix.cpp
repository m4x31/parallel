#include "omp.h"
#include <iostream>
using namespace std;


int cou = 0;

float genRandomFloat() {
  return 1.0 / ((rand() % 999) + 1);
  //cou++;
  return cou * 1.0;
}


int main(int argc, char *argv[]){
  //  #pragma omp parallel
  //  {
    int ID = 0;
    int widthA = stoi(argv[1]);
    int heightA = stoi(argv[2]);

    int widthB = stoi(argv[3]);
    int heightB = stoi(argv[4]);

    int widthResult = widthA;
    int heightResult = heightB;

    
    bool log = false;;

    if (argc > 5) {
      log = true;
    }

    if(widthB != heightA || heightB != widthA) {
      cout << "invalid matrix dimensions" << endl;
      return 0;
    }


    // float **matrixB; 
    // matrixB = new float* [widthB];  
    float matrixA [ heightA ][widthA];
    float matrixB [ heightB ][ widthB ];
    
    float resultMatrix [ heightResult ][ widthResult ];

    // build matrix A
    for (int i = 0; i < heightA; i++) {
      for (int j = 0; j <widthA; j++) {
      	matrixA[i][j] = genRandomFloat();
      }
    }

    
    // build matrix B
    for (int i = 0; i < heightB; i++) {
      for (int j = 0; j < widthB; j++) {
      	matrixB[i][j] = genRandomFloat();
      }
    }

    //log matrix A
    if(log) {
      cout << "Matrix A" << endl;
      for (int i = 0; i < heightA; ++i) {
        for (int j = 0; j < widthA; ++j) {
          std::cout << matrixA[i][j] << ' ';
        }
        std::cout << std::endl;
      }
      cout << endl; 
    }

    // log matrix B
    if(log) {
      cout << "Matrix B" << endl;
      for (int i = 0; i < heightB; ++i) {
        for (int j = 0; j < widthB; ++j) {
          std::cout << matrixB[i][j] << ' ';
        }
        std::cout << std::endl;
      }
      cout << endl;
    }


    // calculate matrix
    int i,j,k;    
    #pragma omp parallel private(i,j,k) shared(matrixA, matrixB, resultMatrix)
    {

    #pragma omp for schedule(auto) collapse(2)

      for (i = 0; i < heightResult; ++i) {
      	for (j = 0; j < widthResult; ++j) {
	  
          float sum = 0;
        
          for(k = 0; k < heightA; k++) {
            // make sure it really uses more than one thread
            // int ID = omp_get_thread_num();
            // cout << ID;
            sum += matrixA[k][j] * matrixB[i][k];
          }
          
          resultMatrix[i][j] = sum;
        }
      }
    }  
    
    // log result Matrix
    if(log) {
      cout << "Result" << endl;
      for (int i = 0; i < heightResult; ++i) {
        for (int j = 0; j < widthResult; ++j) {
          std::cout << resultMatrix[i][j] << ' ';
        }
        std::cout << std::endl;
      }
    }
    
}
