#pragma once
#include <iostream>
#include <array>

const double time_step = 0.1;
// parameter that governs the extent to which differences in reported number
// of sexual partners between males and females is male (1) or female (0) driven
const double theta = 0.5;

// used in addBirths
const double nncirc_prop = 0.1; // Proportion of births that are circumcised
const double propMale = 0.5; // Proportion of births that are male.


const int pop_cols = 13;
const int pop_rows = 82944;

// compartmental indices for pop frame. order is important
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

// useful iterators (could use iota to construct from n* but whatever)
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


extern double popCount[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt];
extern double popDiff[nHIV][nAge][nMale][nRisk][nCD4][nVl][nCirc][nPrep][nCondom][nArt];

void initPop(std::string filename);
void writePop(std::string filename, int timeStep);

// write a pop iterator?





