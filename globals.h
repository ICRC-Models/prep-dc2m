#pragma once
#include <iostream>
#include <array>
// #include <eigen3/Eigen/Dense>

const double time_step = 0.1;
// parameter that governs the extent to which differences in reported number
// of sexual partners between males and females is male (1) or female (0) driven
const double theta = 0.5;

// used in addBirths
const double nncirc_prop = 0.1; // Proportion of births that are circumcised
const double propMale = 0.5; // Proportion of births that are male.

// used in transmit
const double psiCirc = 0.60;
const double psiPrEP = 0.92;
const double psiCondom = 0.78;


const int pop_cols = 13;
const int pop_rows = 82944;

const int risk_cols = 4;
const int risk_rows = 72;

extern Eigen::MatrixXd risk_props_mat;

// compartmental indices for pop frame. order is important
// these are so far unused...
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

// compartmental sizes pop frame.  Order is important!
const int nHIV = 2;
const int nAge = 12;
const int nMale = 2;
const int nRisk = 3;
const int nCD4 = 6;
const int nVl = 6;
const int nCirc = 2;
const int nPrep = 2;
const int nCondom = 2;
const int nArt = 2;

// useful iterators (could use iota to construct from n* sizes above but whatever)
const std::array<int, nHIV> hivBins = {0,1};
const std::array<int, nAge> ageBins = {0,1,2,3,4,5,6,7,8,9,10,11};
const std::array<int, nMale> maleBins = {0,1};
const std::array<int, nRisk> riskBins = {0,1,2};
const std::array<int, nCD4> cd4Bins = {0,1,2,3,4,5};
const std::array<int, nVl> vlBins = {0,1,2,3,4,5};
const std::array<int, nCirc> circBins = {0,1};
const std::array<int, nPrep> prepBins = {0,1};
const std::array<int, nCondom> condomBins = {0,1};
const std::array<int, nArt> artBins = {0,1};

// set in globals.cpp
// these are the index definitions!
extern double popCount[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt];
extern double popDiff[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt];

// initialized and used in transmit.cpp
extern double mixMat[nAge][nMale][nRisk][nAge][nMale][nRisk];
extern double  adjustedPartnersMat[nAge][nMale][nRisk][nAge][nMale][nRisk];
extern double lambdaMat[nAge][nMale][nRisk];

void initPop(std::string filename);
void writePop(std::string filename, int timeStep);
void writeMixMat(int timeStep);
void writeAdjustedPartnersMat(int timeStep);
void writeLambdaMat(int timeStep);

// declare and initialize all intermediate arrays used in various
// routines these things will eventually get created by load parameters
// but for now lets provide an initializatoin funtion

// extern double back_mort[nAge][nMale]; // subtract deaths
// extern double hiv_mort[nAge][nCD4][nArt]; // subtract deaths

// used in addBirths
extern double fert[nAge][nCD4][nArt];
extern double back_mort[nAge][nMale];
extern double hiv_mort[nAge][nCD4][nArt];
extern double cd4_prog[nMale][nVl-1][nCD4-1];
extern double vl_prog[nMale][nVl-1][nCD4-1];

void initParams();
// write a pop iterator?, with fixable indices?





