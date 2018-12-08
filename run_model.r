##################################################
## Allen Roberts
## July 1, 2015
##################################################

rm(list = ls())

library(data.table)
library(reshape2)

## Run name
date <- Sys.Date()
name <- "low_fertility_moultrie_1990_back_mort_reduction"
dir.create(paste0("output/", date), recursive = TRUE)

## Global variables
year_start <- 1980
year_end <- 2020
tstep <- 0.1 # years
nsteps <- (year_end - year_start + 1) / tstep

## Attribute values
hiv <- c(0, 1)
age <- seq(1, 12)

print("age")
print(age)

male <- c(0, 1)
risk <- seq(1, 3)
cd4 <- seq(0, 5) ## CD4/VL = 0 right now means that they are uninfected
vl <- seq(0, 5)
circ <- c(0, 1)
prep <- c(0, 1)
condom <- c(0, 1)
art <- c(0, 1)

## Variable for all of the dimensions in our model.  Usefully for switching the data.table keys
all_keys <- c("hiv", "age", "male", "risk", "cd4", "vl", "circ", "prep", "condom", "art")

## Source functions
source("interpolate.r")
source("demography_functions.r")
source("progressDisease.r")
source("seedInfections.r")
source("riskAdjust.r")
source("lambda_functions.r")
source("transmit.r")
source("distributeART.r")
source("distributeCondoms.r")

## Load input epidemiological parameters
source("load_parameters.r")

## Initialize population matrix
pop <- as.data.table(expand.grid(sapply(all_keys, get)))
setattr(pop, 'names', c("hiv", "age", "male", "risk", "cd4", "vl", "circ", "prep", "condom", "art"))
pop$count <- 0
pop$diff <- 0

## Data tables for statistics
population <- as.data.table(expand.grid(hiv, age, male, seq(1, nsteps)))
setattr(population, 'names', c("hiv", "age", "male", "time"))
population[, pop_size := 0]
setkey(population, time, hiv, age, male)

births <- as.data.table(expand.grid(hiv, age, seq(1, nsteps)))
setattr(births, 'names', c("hiv", "age", "time"))
births[, num_births := 0]

deaths <- as.data.table(expand.grid(hiv, age, male, seq(1, nsteps)))
setattr(deaths, 'names', c("hiv", "age", "male", "time"))
deaths[, c("back_deaths", "hiv_deaths") := 0]

## Incidence
incidence <- as.data.table(expand.grid(age, male, seq(1, nsteps)))
setattr(incidence, 'names', c("age", "male", "time"))
incidence[, c("horiz_infections", "vert_infections") := 0]

## Distribution of CD4 and VL
dis_dist <- as.data.table(expand.grid(age, male, cd4, vl, art, seq(1, nsteps)))
setattr(dis_dist, 'names', c("age", "male", "cd4", "vl", "art", "time"))
dis_dist[, total := 0]
setkey(dis_dist, time, age, male, cd4, vl, art)

## Interventions
interventions <- as.data.table(expand.grid(hiv, age, male, art, condom, circ, seq(1, nsteps)))
setattr(interventions, 'names', c("hiv", "age", "male", "art", "condom", "circ", "time"))
interventions[, total := 0]
setkey(interventions, time, hiv, age, male, art, condom, circ)

## initialization set outsize loop
setkey(pop, hiv, age, male, cd4, vl, circ, prep, condom, art)
pop[init_pop, count := pop]

## Distribute by risk group
setkey(pop, age, male, risk)
pop[risk_props, count := count * prop]

## Seed infections - this is currently adding 0.1% of total population to infected groups, but not subtracting them from the susceptible pool.  Need to confirm with Roger
seedInfections(pop, 0.001)

nsteps = 1

setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
print("Before the loop")
print(pop[41569, ])

## Run model
for(tt in 1:nsteps) {

  ## tt <- 410
  print(tt)

  pop[, time := (tt-1)] # add time column to pop? why if you're passsing tt?
  ## Calculate calendar year
  year <- floor(year_start + (tt - 1) * tstep)

  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)

  fwrite(pop, col.names=FALSE, file=paste0("pop_", (tt-1), ".out"))
  
  ## Distribute ART coverage
  start_time <- Sys.time()
  distributeART(pop, tt)
  end_time <- Sys.time()
  print("distribute art time")
  print(end_time - start_time)
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  print("shitty index?")
  fwrite(pop, col.names=FALSE, file=paste0("distributeART_", (tt-1), ".out"))
  ## Distribute condom coverage
  start_time <- Sys.time()
  distributeCondoms(pop, tt)
  end_time <- Sys.time()
  print("distribute condoms time")
  print(end_time - start_time)
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  fwrite(pop, col.names=FALSE, file=paste0("distributeCondoms_", (tt-1), ".out"))

  ## Calculate statistics
  ## Populations
  pop_stats <- pop[, list(size = sum(count)), by = list(hiv, age, male, time)]
  setkey(pop_stats, time, hiv, age, male)
  population[pop_stats, pop_size := size]

  ## Disease distribution
  dis_stats <- pop[hiv == 1, list(size = sum(count)), by = list(art, age, male, cd4, vl, time)]
  setkey(dis_stats, time, age, male, cd4, vl, art)
  dis_dist[dis_stats, total := size]

  ## Intervention coverage
  int_stats <- pop[, list(size = sum(count)), by = list(hiv, age, male, art, condom, circ, time)]
  setkey(int_stats, time, hiv, age, male, art, condom, circ)
  interventions[int_stats, total := size]

  # # Optional reduction in background mortality
  # if(year >= 1990) {
  #   back_mort[, mu := mu * (100 - 2 * tstep)/100]
  # }

  ## Optional decrease in fertility
  # fert[, gamma := gamma * (100 - tstep)/ 100]

  ## Demography
  addBirths(pop)
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  fwrite(pop, col.names=FALSE, file=paste0("addBirths_", (tt-1), ".out"))
  
  subtractDeaths(pop)
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  fwrite(pop, col.names=FALSE, file=paste0("subtractDeaths_", (tt-1), ".out"))
  
  
  agePop(pop, tstep)
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  fwrite(pop, col.names=FALSE, file=paste0("agePop_", (tt-1), ".out"))

  ## Disease progression
  progressDisease(pop, tstep)
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  fwrite(pop, col.names=FALSE, file=paste0("progressDisease_", (tt-1), ".out"))
  
  ## Transmission
  ## Calculate the mixing matrix
  start_time <- Sys.time()
  calcMixMat(pop, mixing_matrix, tt)
  end_time <- Sys.time()
  print("calcMixMat time")
  print(end_time - start_time)
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  ## fwrite(pop, col.names=FALSE, file="calcMixMat.out")
  
  setorder(mixing_matrix, age, male, risk, age_p, male_p, risk_p)
  setcolorder(mixing_matrix, c("age", "male", "risk", "age_p", "male_p", "risk_p", "prop"))
  fwrite(mixing_matrix, col.names = FALSE, file = paste0("mixing_matrix_", (tt-1), ".out"))

  ## Calculate adjusted partnerships per year
  start_time <- Sys.time() 
  adjustPartnerships(pop, mixing_matrix)
  end_time <- Sys.time()
  print("adjustPartnerships time")
  print(end_time - start_time)
  setorder(adjusted_partners, age, male, risk, age_p, risk_p)
  setcolorder(adjusted_partners, c("age", "male", "risk", "age_p", "risk_p", "adjusted_partners"))
  fwrite(adjusted_partners, col.names = FALSE, file = paste0("adjusted_partners_", (tt-1), ".out"))
  
  ## Calculate lambda
  start_time <- Sys.time() 
  calcLambda(pop, mixing_matrix, adjusted_partners)
  end_time <- Sys.time()
  print("calcLambda time")
  print(end_time - start_time)
  
  setorder(lambda_mat, age, male, risk)
  setcolorder(lambda_mat, c("age", "male", "risk", "lambda"))
  fwrite(lambda_mat, col.names = FALSE, file = paste0("lambda_mat_", (tt-1), ".out"))
  
  ## Transmit infections
  start_time <- Sys.time() 
  transmit(pop, lambda_mat)
  end_time <- Sys.time()
  print("transmit time")
  print(end_time - start_time)
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  fwrite(pop, col.names=FALSE, file=paste0("transmit_", (tt-1), ".out"))
  
  # Compute end-of-year population and set difference back to zero for next iteration of loop
  pop[, c("count", "diff") := list(count + diff, 0)]
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  fwrite(pop, col.names=FALSE, file=paste0("endPop_", (tt-1), ".out"))
  
  
  # Adjust population to match risk prevalence
  start_time <- Sys.time() 
  riskAdjust(pop)
  end_time <- Sys.time()
  print("riskAdjust time")
  print(end_time - start_time)
  setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)
  fwrite(pop, col.names=FALSE, file=paste0("riskAdjust_", (tt-1), ".out"))
  # Increment time step
  tt <- tt + 1

}

## Save results
save(population, births, deaths, incidence, interventions, dis_dist, tstep, year_start, year_end, date, name, file = paste("output/", date, "/", name, ".RData", sep = ""))

# ## Plot results
# source("plot_results.r")

