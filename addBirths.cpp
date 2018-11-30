#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 addBirths.cpp csvUtil.cpp

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);

int pop_cols = 13;
int pop_rows = 82944;

const int hivInd = 0;
const int ageInd = 1;
const int maleInd = 2;
const int riskInd = 3;
const int cd4Ind = 4;
const int vlInd = 5;
const int circInd = 6;
const int prepInd = 7;
const int condomInd = 8;
const int artInd = 9;
const int countInd = 10;
const int diffInd = 11;
const int timeInd = 12;

double nncirc_prop = 0.1; // Neonatal circumcision prevalence - shouldn't we move this outside at some point?

// fert header: age,male,gamma,cd4,art
int fert_cols = 5;
int fert_rows = 144;
Eigen::MatrixXd fert_mat = readCSV("fert.csv", fert_cols, fert_rows); // age, male, gamma, cd4, art

const int gammaInd = 2; // Column of fert_mat that contains fertility coefficient (zero indexed)

void addBirths(Eigen::MatrixXd &pop){

	// Constants for pop frame. Maybe we should move these outside later?
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
    const int nPopRows = pop.rows();

    // Set up fert array - ignore male column. Notice this reorders with respect to R output
    double fert[nAge][nCD4][nArt] = {0};
    int rowInd;

    for(int ii = 0; ii < nAge; ii++) {
    	for(int jj = 0; jj < nCD4; jj++){
    		for(int kk = 0; kk < nArt; kk++) {

    			rowInd = nAge * ii + nArt * jj + kk;

    			// std::cout << "ii : " << ii << std::endl;
    			// std::cout << "jj : " << jj << std::endl;
    			// std::cout << "kk : " << kk << std::endl;
    			// std::cout << "row Ind: " << rowInd << std::endl;
    			// std::cout << "gamma: " << fert_mat(rowInd, gammaInd) << std::endl;
    			fert[ii][jj][kk] = fert_mat(rowInd, gammaInd); // This line requires fert_mat to be ordered by age, cd4, and ART status. 
    		}

    	}
    }
    

    // Find female indicators in pop - is push_back slow? We know in advance how many indicators have male == 0
     std::vector<int> maleInds0;
    for (int ii = 0; ii<nPopRows; ii++){
        double test = pop(ii, maleInd);
        if (test > 0){
            // std::cout << "test " << test <<std::endl;
        }
        if (pop(ii, maleInd) == 0){

            maleInds0.push_back(ii);
        }
    }

	// Calculate total birhts from HIV- and HIV+ mothers by multiplying gamma in fert by count. Should only operate on women

    double births_from_neg = 0;
    double births_from_pos = 0;
    double gamma; 
    double count;

    int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom, iart;

    for (int ii=0; ii<nPopRows; ii++){ // Could shorten this loop by only looping over rows for which male == 0?
        // auto pop_row = pop.row(ii);
        // std::cout << "row " << ii << " of " << nPopRows << std::endl;
        count = pop(ii,countInd);
        if (ii==0){
            std::cout << "first line count " << count << std::endl;
        }
        // std:: cout << "count: " << count << std::endl;
        ihiv = pop(ii,hivInd);
        iage = pop(ii,ageInd) - 1; // 1 indexed fucker
        imale = pop(ii,maleInd);
        irisk = pop(ii,riskInd) - 1; // 1 indexed fucker
        icd4 = pop(ii,cd4Ind);
        ivl = pop(ii,vlInd);
        icirc = pop(ii,circInd);
        iprep = pop(ii,prepInd);
        icondom = pop(ii,condomInd);
        iart = pop(ii, artInd);

        if(imale == 0) { // Only affects women

        	// Find corresponding fertility coefficient
        	gamma = fert[iage][icd4][iart];
        	

        	if(ihiv == 0 | iart == 1) { // Treats HIV+ women with ART == 1 to be negative - could change this later.
        		births_from_neg += gamma*count;
        	}
     		else {
     			births_from_pos += gamma*count;
     		}
        }

    }

    std::cout << "births_from_neg: " << births_from_neg << std::endl;
    std::cout << "births_from_pos: " << births_from_pos << std::endl;

// Calculate total infants born HIV+

// Calculate total infants born HIV-

// Distribute births added by sex

// Distribute births added by circumcision status

// Distribute births across risk status

//

}

int main(){
    clock_t tStart;
    clock_t tEnd;
    Eigen::MatrixXd pop = readCSV("distributeCondoms.out", pop_cols, pop_rows);
    tStart = clock();
    addBirths(pop);
    tEnd = clock();
    std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
    writeCSV(pop, "addBirths.cout");
}
