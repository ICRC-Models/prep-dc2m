##################################################
## Allen Roberts
## July 7, 2015
## Description: Function to seed initial HIV+ infections into the model
##################################################

## This function adds HIV+ infections to the initial population.  Currently it does not subtract those infections from the HIV- population.  "prop" is the proportion of the TOTAL population that will be infected.  These are distributed equally across the two different groups.

seedInfections <- function(dt, prop) {
  
  ## Requires variable naming conventions
  ## prop is the proportion of the population that will be added to the infectious group
  # dt <- copy(pop)
  # prop <- 0.001
  
  ## Total population
  N <- sum(dt$count)
  ## Divide equally across males and females
  N <- N/2
  
  ## Distribute evenly among males and females
  # dt[hiv == 1 & risk == 2 & cd4 == 1 & vl == 1 & circ == 0 & prep == 0 & condom == 0 & art == 0 & ((male == 1 & age == 6 ) | (male == 0 & age == 5)), count := N * prop / 2]
  
  ## New distribution method:  
   ## Infect a total of 0.1% of the total male population across age groups 4-6, risk groups 2-3, and a total of 0.1% of the total female population across age groups 5-6 and risk groups 2-3. For each gender, equal numbers of individuals are infected in each of the initially infected compartments.
  
  num_cat_male <- 3*2 ## Number of compartments across which the total male infected population will be spread equally
  num_cat_female <- 2*2 ## Number of compartments across which the total female infected population will be spread equally
  ## Add/subtract males
  male_hiv <- dt[male == 1 & hiv == 1 & age %in% seq(4, 6) & circ == 0 & prep == 0 & condom == 0 & art == 0 & vl == 2 & cd4 == 2 & risk %in% c(2, 3)]
  setkey(male_hiv, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  setkey(dt,  hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  dt[male_hiv, count := count + prop * N / num_cat_male]
  
  male_hiv[, c("hiv", "vl", "cd4") := 0]
  dt[male_hiv, count := count - prop * N / num_cat_male]

  
  ## Add/subtract females
  female_hiv <- dt[male == 0 & hiv == 1 & age %in% seq(5, 6) & circ == 0 & prep == 0 & condom == 0 & art == 0 & vl == 2 & cd4 == 2 & risk %in% c(2, 3)]
  setkey(female_hiv, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  setkey(dt,  hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  dt[female_hiv, count := count + prop * N / num_cat_female]
  
  female_hiv[, c("hiv", "vl", "cd4") := 0]
  dt[female_hiv, count := count - prop * N / num_cat_female]
  
}
