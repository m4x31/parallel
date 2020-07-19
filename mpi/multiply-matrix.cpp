#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <vector>
using namespace std;

const int TAG_MATRIX_A_HEIGHT = 101;
const int TAG_MATRIX_A_WIDTH = 102;
const int TAG_MATRIX_A_DATA = 103;

const int TAG_MATRIX_B_HEIGHT = 111;
const int TAG_MATRIX_B_WIDTH = 112;
const int TAG_MATRIX_B_DATA = 113;

bool logg = false;

class MatrixDimension {
  public:
    int height, width;

    MatrixDimension(int h, int w) {
      height = h;
      width = w;
    }
};

class MatrixWithDimension {
  public:
    int** matrix = NULL;
    MatrixDimension dimension = MatrixDimension(0,0);

    MatrixWithDimension(int** m, MatrixDimension d) {
      matrix = m;
      dimension = d;
    }
};

int ccc = 1;


int **build_matrix(MatrixDimension dim) {
  //manually reserve mem for easier mpi sending

  int* row = (int *)malloc(dim.height * dim.width * sizeof(int));
  int** matrix = (int **)malloc(dim.height * sizeof(int*));
  for (int i = 0; i< dim.height; i++) {
    matrix[i] = &(row[dim.width * i]);
    for(int j = 0; j < dim.width; j++) {
      matrix[i][j] = ccc;
      ccc++;
    }
  }

  return matrix;
}

void log_matrix(int **matrix, MatrixDimension dim) {

  string l = "";
  if(logg) {
    for (int i = 0; i < dim.height; ++i) {
      for (int j = 0; j < dim.width; ++j) {
        cout << matrix[i][j] << ' ';
      }
      cout << endl;
    }
    cout << endl;  
  }

}

void mutiply_matrix(int** matrix_a, MatrixDimension dim_a, int** matrix_b, MatrixDimension dim_b, int** resultMatrix ) {

  for(int i =  0; i < dim_b.height; i++) {
    for(int j = 0; j < dim_b.width; j++) {
      int sum = 0;

      for(int k = 0; k < dim_b.width; k++) {
        sum = matrix_a[k][j] * matrix_b[i][k];
      }

      resultMatrix[i][j] = sum;
    }
  }
  return;
}



vector<MatrixWithDimension*> split_matrices(int** matrix, MatrixDimension dim) {

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  size--;

  int matrix_edge_length = (dim.height + size - 1) / size;

  vector<MatrixWithDimension *> matrices;



  for(int i = 0; i < size; i++) {

    MatrixDimension d = MatrixDimension(matrix_edge_length, dim.width);


    int **m = build_matrix(d);


    for(int j = 0; j < matrix_edge_length; j++) {
      

      for(int k = 0; k < dim.width; k++ ) {

        m[j][k] = matrix[i*matrix_edge_length+j][k];

      }
    }

    matrices.push_back( new MatrixWithDimension(m, d) );
  }

  return matrices;
}



void run_primary(int heightA, int widthA, int heightB, int widthB) {

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  size--;

  if ((heightB % size != 0) || heightB < size) {
    heightB = heightB + (size - (heightB % size));
  }



  
  MatrixDimension aDim = MatrixDimension(heightA, widthA);
  MatrixDimension bDim = MatrixDimension(heightB, widthB);
  MatrixDimension resultDim = MatrixDimension(bDim.height, aDim.width);



  int** matrixA = build_matrix(aDim);
  log_matrix(matrixA, aDim);

  int** matrixB = build_matrix(bDim);
  log_matrix(matrixB, bDim);

  int** matrixResult = build_matrix(resultDim);
  log_matrix(matrixResult, resultDim);



  vector<MatrixWithDimension*> matrices = split_matrices(matrixB, bDim);

  for(int i = 0; i < size; i++) {

    MPI_Send(&(aDim.height), 1, MPI_INT, i+1, TAG_MATRIX_A_HEIGHT, MPI_COMM_WORLD);
    MPI_Send(&(aDim.width), 1, MPI_INT, i+1, TAG_MATRIX_A_WIDTH, MPI_COMM_WORLD);

    int **matrixB = matrices[i] -> matrix;
    MatrixDimension bDim = matrices[i] -> dimension;

    MPI_Send(&(matrices[i] -> dimension.height), 1, MPI_INT, i+1, TAG_MATRIX_B_HEIGHT, MPI_COMM_WORLD);
    MPI_Send(&(matrices[i] -> dimension.width), 1, MPI_INT, i+1, TAG_MATRIX_B_WIDTH, MPI_COMM_WORLD);

    MPI_Send(&(matrixA[0][0]), aDim.width*aDim.height, MPI_INT, i+1, TAG_MATRIX_A_DATA, MPI_COMM_WORLD);
    MPI_Send(&(matrixB[0][0]), bDim.height*bDim.width, MPI_INT, i+1, TAG_MATRIX_B_DATA, MPI_COMM_WORLD);


  }
}




void run_secondary(int rank) {


  MPI_Status status;

  MatrixDimension dim_a  = MatrixDimension(0,0);
  MPI_Recv(&(dim_a.height), 1, MPI_INT, 0, TAG_MATRIX_A_HEIGHT, MPI_COMM_WORLD, &status);
  MPI_Recv(&(dim_a.width), 1, MPI_INT, 0, TAG_MATRIX_A_WIDTH, MPI_COMM_WORLD, &status);

  MatrixDimension dim_b  = MatrixDimension(0,0);
  MPI_Recv(&(dim_b.height), 1, MPI_INT, 0, TAG_MATRIX_B_HEIGHT, MPI_COMM_WORLD, &status);
  MPI_Recv(&(dim_b.width), 1, MPI_INT, 0, TAG_MATRIX_B_WIDTH, MPI_COMM_WORLD, &status);

  int **matrixA = build_matrix(dim_a);
  int **matrixB = build_matrix(dim_b);

  MPI_Recv(&(matrixA[0][0]), dim_a.height * dim_a.width, MPI_INT, 0, TAG_MATRIX_A_DATA, MPI_COMM_WORLD, &status);
  MPI_Recv(&(matrixB[0][0]), dim_b.height * dim_b.width, MPI_INT, 0, TAG_MATRIX_B_DATA, MPI_COMM_WORLD, &status);

  log_matrix(matrixA, dim_a);
  log_matrix(matrixB, dim_b);

  MatrixDimension resultDimension = MatrixDimension(dim_b.height, dim_a.width);
  int **resultMatrix = build_matrix(resultDimension);

  mutiply_matrix(matrixA, dim_a, matrixB, dim_b, resultMatrix);
  
  cout << "secondary #" << rank << " result" << endl;
  log_matrix(resultMatrix, resultDimension);
  cout << "------------------------------" << endl;

}



int main(int argc, char **argv) {

  int widthA = stoi(argv[1]);
  int heightA = stoi(argv[2]);

  int widthB = stoi(argv[3]);
  int heightB = stoi(argv[4]);
 

  if (argc > 5) {
    logg = true;
  }
  
  
  int size, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(size == 1) {
    cout << "invalid cluster size";
    exit(0);
  }

  if(rank == 0) {
    run_primary(heightA, widthA, heightB, widthB);
  } else {
    run_secondary(rank);
  }
  
  // printf("SIZE = %d RANK = %d\n",size,rank);
  MPI_Finalize();
  return(0);
}
