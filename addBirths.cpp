#include <iostream>
#include <vector>
#include <time.h>
#include <eigen3/Eigen/Dense>

// clang++ -O3 -std=c++11 addBirths.cpp csvUtil.cpp

// prototype -- move to header eventually
Eigen::MatrixXd readCSV(std::string filename, int cols, int rows);
void writeCSV(Eigen::MatrixXd matrix, std::string filename);

// int pop_cols = 13;
// int pop_rows = 82944;

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

const double nncirc_prop = 0.1; // Proportion of births that are circumcised
const double propMale = 0.5; // Proportion of births that are male. 


// fert header: age,male,gamma,cd4,art
int fert_cols = 5;
int fert_rows = 144;
Eigen::MatrixXd fert_mat = readCSV("fert.csv", fert_cols, fert_rows); // age, male, gamma, cd4, art
const int gammaInd = 2; // Column of fert_mat that contains fertility coefficient (zero indexed)

// Vertical transmission parameters - these are time-varying
int vert_cols = 1;
int vert_rows = 410;
Eigen::MatrixXd vert_trans_mat = readCSV("vert_trans.csv", vert_cols, vert_rows);

// Risk proportions: needed for distributing births by risk status. Only actually need the first 12 rows but not sure how to read only those ones in.
int risk_cols = 4;
int risk_rows = 72;
Eigen::MatrixXd risk_props_mat = readCSV("risk_props.csv", risk_cols, risk_rows);
const double risk_prop_ind = 0; // Column of risk_mat that contains risk proportions (zero indexed)


void addBirths(Eigen::MatrixXd &pop, int time_index){

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

    // Vertical transmission paramter
    const double vert_trans = vert_trans_mat(time_index);
    // std::cout << "vert_trans: " << vert_trans << std::endl;

    // Set up fert array - ignore male column. Notice this reorders with respect to R output
    double fert[nAge][nCD4][nArt] = {0};
    int rowInd;

    for(int ii = 0; ii < nAge; ii++) {
    	for(int jj = 0; jj < nCD4; jj++){
    		for(int kk = 0; kk < nArt; kk++) {

    			rowInd = ii * (nCD4*nArt) + jj * nArt + kk;

    			// std::cout << "ii : " << ii << std::endl;
    			// std::cout << "jj : " << jj << std::endl;
    			// std::cout << "kk : " << kk << std::endl;
    			// std::cout << "row Ind: " << rowInd << std::endl;
    			// std::cout << "gamma: " << fert_mat(rowInd, gammaInd) << std::endl;
    			fert[ii][jj][kk] = fert_mat(rowInd, gammaInd); // This line requires fert_mat to be ordered by age, cd4, and ART status. 
    		}

    	}
    }
    

    // Find female and baby indicators in pop - is push_back slow? We know in advance how many indicators have male == 0
     std::vector<int> maleInds0;
     std::vector<int> ageInds0;
    for (int ii = 0; ii<nPopRows; ii++){

    	// Check if male
        if (pop(ii, maleInd) == 0){

            maleInds0.push_back(ii);
        }

        // Check if baby
        if (pop(ii, ageInd) == 1){

            ageInds0.push_back(ii);
        }
    }

	// Calculate total birhts from HIV- and HIV+ mothers by multiplying gamma in fert by count. Should only operate on women

    double births_from_neg = 0;
    double births_from_pos = 0;
    double gamma; 
    double count;

    int ihiv, iage, imale, irisk, icd4, ivl, icirc, iprep, icondom, iart;

    for (int ii : maleInds0){ 

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

// Calculate total infants born HIV+ and HIV-
  double neg_births;
  double pos_births;

  neg_births = births_from_pos * (1 - vert_trans) + births_from_neg;
  pos_births = births_from_pos * vert_trans;

  std::cout << "neg_births: " << neg_births << std::endl;
  std::cout << "pos_births: " << pos_births << std::endl;


// Distribute births into pop table

// For use in the loop
  double riskProp; 

  for (int rowInd : ageInds0){

        ihiv = pop(rowInd, hivInd);
        iage = pop(rowInd, ageInd) - 1; // 1 indexed fucker
        imale = pop(rowInd, maleInd);
        irisk = pop(rowInd, riskInd) - 1; // 1 indexed fucker
        icd4 = pop(rowInd, cd4Ind);
        ivl = pop(rowInd, vlInd);
        icirc = pop(rowInd, circInd);
        iprep = pop(rowInd, prepInd);
        icondom = pop(rowInd, condomInd);
        iart = pop(rowInd, artInd);

        double diff = 0; // Tracker for number of births
        double mult = 1; // Multiplier for allocating proportions
        // std::cout << "iage: " << iage << std::endl;

        // Births are only allocated into these compartments. Could maybe search for these indicators outside
     	if(iprep == 0 & icondom == 0 & iart == 0) {
     		 

	        // Find proportion of births going into this risk group
	        riskProp = risk_props_mat(imale * nRisk + irisk, risk_prop_ind);
	        mult = mult * riskProp;

	        // Allocate across sex and circumcision status
	        if(imale == 1) {
	        	mult = mult * propMale;

	        	if(icirc == 1) {
	        		mult = mult * nncirc_prop;
	        	} else {
	        		mult = mult * (1-nncirc_prop);
	        	}
	        } else {

	        	mult = mult * (1-propMale);

	        	if(icirc == 1) { // No females circumcised
	        		mult = 0;
	        	} 
	        }

	        // Allocate across HIV status 
	        if(ihiv == 0) {
	        	if(ivl == 0 & icd4 == 0 & iart == 0) {
	        		diff = neg_births * mult;
	        	} 
	        } else {
	        	if(ivl == 1 & icd4 == 1 & iart == 0) {
	        		diff = pos_births * mult;
	        	} 
	        }

	         // if(diff > 1) {
	         // 	std::cout << "male: " << imale << std::endl; 
	         // 	std::cout << "risk: " << irisk << std::endl;
	         // 	std::cout << "diff: " << diff << std::endl;
	         // }


  		// std:: cout << "rowInd: " << rowInd << std::endl;
  		// std:: cout << "abs: " << abs(rowInd -  577) << std::endl;
	       
	        // Add to diff column of pop
	         pop(rowInd, diffInd) += diff; 
	     }


    }


}

// int main(){
//     clock_t tStart;
//     clock_t tEnd;
//     Eigen::MatrixXd pop = readCSV("distributeCondoms.out", pop_cols, pop_rows);
//     tStart = clock();
//     addBirths(pop, 409);
//     tEnd = clock();
//     std::cout << "time took: " << (double)(tEnd - tStart)/CLOCKS_PER_SEC << std::endl;
//     writeCSV(pop, "addBirths.cout");
// }
