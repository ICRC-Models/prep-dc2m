#include <iostream>
#include <fstream>
#include <eigen3/Eigen/Dense>

// define the format you want, you only need one instance of this...
// std::cout.precision(5);
const static Eigen::IOFormat CSVFormat(Eigen::FullPrecision, Eigen::DontAlignCols, ",", "\n");

// csv header: hiv,age,male,risk,cd4,vl,circ,prep,condom,art,count,diff
// int cols = 12;
// int rows = 82944;

void writeCSV(Eigen::MatrixXd matrix, std::string filename){
    std::ofstream file(filename);
    file << matrix.format(CSVFormat);
}

// https://gist.github.com/infusion/43bd2aa421790d5b4582
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows, bool header = false) {
  std::cout.precision(20);
  std::ifstream in(filename);

  std::string line;

  int row = 0;
  int col = 0;


  Eigen::MatrixXd res = Eigen::MatrixXd(rows, cols);

  if (in.is_open()) {

    while (std::getline(in, line)) {

      if(header) {

        header = false;
        continue;
      }

      char *ptr = (char *) line.c_str();
      int len = line.length();

      col = 0;

      char *start = ptr;
      for (int i = 0; i < len; i++) {

        if (ptr[i] == ',') {
          res(row, col++) = atof(start);
          start = ptr + i + 1;
        }
      }
      res(row, col) = atof(start);

      row++;
    }

    in.close();
  }
  return res;
}

Eigen::MatrixXd readCSVOld(std::string filename, int cols, int rows){
  std::cout.precision(20);
  Eigen::MatrixXd res = Eigen::MatrixXd(rows, cols);
  std::ifstream indata;

  indata.open(filename);

  std::string line;
  int row = 0;
  while (getline(indata, line))
  {
    std::stringstream lineStream(line);
    std::string cell;
    int col = 0;
    while (std::getline(lineStream, cell, ','))
    {
      double tmp = std::stod(cell);
      // std::cout << "cell " << cell << " " << tmp << std::endl;
      res(row,col) = tmp;
      col++;
    }
  row++;
  if (row > 100){
    break;
  }
  }
  return res;
}
