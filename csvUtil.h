#pragma once
#include <eigen3/Eigen/Dense>
#include "globals.h"

void writeCSV(Eigen::MatrixXd matrix, std::string filename);
// https://gist.github.com/infusion/43bd2aa421790d5b4582
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);

