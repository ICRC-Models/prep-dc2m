#include <map>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <array>

// clang++ -O3 -std=c++11 data.cpp csvUtil.cpp

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows, bool header);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);

const int nHIV = 2;
const int nAge = 12;
const int nMale = 2;
const int nRisk = 3;
const int nCD4 = 6; // len of art_cov_row
const int nVl = 6;
const int nCirc = 2;
const int nPrep = 2;
const int nCondom = 2;
const int nArt = 2; // art is 0 or 1

int main() {

	// ART Coverage - by CD4 count and year
	int art_cols = 3;
	int art_rows = 24; // This is excluding the header	
	Eigen::MatrixXd art_cov_data = readCSV("data/art_coverage_cpp.csv", art_cols, art_rows, true);


	// std::cout << art_cov_data << std::endl;

	int cd4Ind = 0;
	int yearInd = 1;
	int propInd = 2;

	std::array<std::vector< std::array<double, 2> >, nCD4 > art_cov_pairs; 

	int cd4;
	double year, prop;

	for(int ii = 0; ii < art_rows; ii ++) {

		cd4 = art_cov_data(ii, cd4Ind);
		year = art_cov_data(ii, yearInd);

		prop = art_cov_data(ii, propInd);

		std::array<double, 2> new_pair = {year, prop}; 

		// std::cout << "[" << new_pair[0] << ", " << new_pair[1] << "]" <<  std::endl;

		art_cov_pairs[cd4].push_back(new_pair);

	}

	// for(auto & row : art_cov_pairs){
	// 	for (auto & tuple : row){
	// 		std::cout << "(" << tuple[0] << ", " << tuple[1] << "), ";
	// 	}
	// 	std::cout << std::endl;
	// }

	// iota - incremented lists

	double art_cov[nCD4][nART][nSteps]

	// Interpolate fills in art cov array for all time steps
	void interpolate(std::vector < std::array<double, 2> >, step_size, int start, int end) {



		// http://www.cplusplus.com/forum/general/216928/

	}

	for(int ii = 0; ii < nCD4; ii ++) {

		interpolate(art_cov_pairs[ii][0], art_cov_pairs[ii][1],  )

	}


	// double art_cov[nCD4][nART][nSteps];  // Coverage by CD4 count and ART for every time steps
}
