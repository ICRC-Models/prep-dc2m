##################################################
## Allen Roberts
## July 1, 2015
##################################################

rm(list = ls())

library(data.table)
library(reshape2)

## Run name
date <- Sys.Date()
name <- "nick_parameters_point01_nohiv"
dir.create(paste0("output/", date), recursive = TRUE)

## Global variables
year_start <- 1980
year_end <- 2020

tstep <- 0.01 # years
nsteps <- (year_end - year_start + 1) / tstep

## Attribute values
hiv <- c(0, 1)
age <- seq(1, 12)
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
population <- as.data.table(expand.grid(hiv, age, male, risk, cd4, vl, art, seq(year_start, year_end, by = 0.1)))
setattr(population, 'names', c("hiv", "age", "male", "risk", "cd4", "vl", "art", "year"))
population[, pop_size := 0]
setkey(population, year, hiv, age, male, risk, cd4, vl, art)
#
# births <- as.data.table(expand.grid(hiv, age, seq(1, nsteps)))
# setattr(births, 'names', c("hiv", "age", "time"))
# births[, num_births := 0]

## Can update this to reduce size (ie, keep track every 0.1 years)
deaths <- as.data.table(expand.grid(hiv, age, male, risk, seq(1, nsteps)))
setattr(deaths, 'names', c("hiv", "age", "male", "risk", "time"))
deaths[, c("back_deaths", "hiv_deaths") := 0]

## Incidence - can update this to reduce size (ie, keep track every 0.1 years)
incidence <- as.data.table(expand.grid(age, male, risk, seq(1, nsteps)))
setattr(incidence, 'names', c("age", "male", "risk", "time"))
incidence[, c("horiz_infections", "vert_infections") := 0]
#
# ## Distribution of CD4 and VL
# dis_dist <- as.data.table(expand.grid(age, male, cd4, vl, art, seq(1, nsteps)))
# setattr(dis_dist, 'names', c("age", "male", "cd4", "vl", "art", "time"))
# dis_dist[, total := 0]
# setkey(dis_dist, time, age, male, cd4, vl, art)
#
# ## Interventions - now by CD4 count
# interventions <- as.data.table(expand.grid(hiv, age, male, cd4, art, condom, circ, seq(1, nsteps)))
# setattr(interventions, 'names', c("hiv", "age", "male", "cd4", "art", "condom", "circ", "time"))
# interventions[, total := 0]
# setkey(interventions, time, hiv, age, male, cd4, art, condom, circ)

## For testing purposes - make this the default order.
setorder(pop, hiv, age, male, risk, cd4, vl, circ, prep, condom, art)

## Add intial populations.  Initially all are susceptible.
setkey(pop, hiv, age, male, cd4, vl, circ, prep, condom, art)
pop[init_pop, count := pop]

## Distribute by risk group
setkey(pop, age, male, risk)
pop[risk_props, count := count * prop]

## Seed infections - this is currently adding 0.1% of total population to infected groups, but not subtracting them from the susceptible pool.  Need to confirm with Roger
seedInfections(pop, 0.001)

## Run model
for(tt in 1:nsteps) {

 ## tt <- 1
  print(tt)

  pop[, time := tt]
  ## Calculate calendar year
  year <- floor(year_start + (tt - 1) * tstep)

  ## Distribute ART coverage
  distributeART(pop, tt)

  ## Distribute condom coverage
  distributeCondoms(pop, tt)

  ## Calculate statistics
  ## Populations
  year_exact <- year_start + (tt - 1) * tstep

  pop_stats <- pop[, list(size = sum(count)), by = list(hiv, age, male, risk, cd4, vl, art)]
  pop_stats[, year := year_exact] ## This is dangerous because it matches on a numeric, which could fail with numerical error
  setkey(pop_stats, year, hiv, age, male, risk, cd4, vl, art)
  population[pop_stats, pop_size := size]

  ## Disease distribution
  # dis_stats <- pop[hiv == 1, list(size = sum(count)), by = list(art, age, male, cd4, vl, time)]
  # setkey(dis_stats, time, age, male, cd4, vl, art)
  # dis_dist[dis_stats, total := size]

  ## Intervention coverage
  # int_stats <- pop[, list(size = sum(count)), by = list(hiv, age, male, cd4, art, condom, circ, time)]
  # setkey(int_stats, time, hiv, age, male, cd4, art, condom, circ)
  # interventions[int_stats, total := size]

  # Optional reduction in background mortality
  # if(year >= 1990) {
  #   back_mort[, mu := mu * (100 - 0.5 * tstep)/100]
  # }

  ## Optional decrease in fertility
  # fert[, gamma := gamma * (100 - tstep)/ 100]

  ## Demography
  addBirths(pop, year = year_exact)
  subtractDeaths(pop)
  agePop(pop, tstep)

  ## Disease progression
  progressDisease(pop, tstep)

  ## Transmission
  ## Calculate the mixing matrix
  calcMixMat(pop, mixing_matrix, tt)

  ## Calculate adjusted partnerships per year
  adjustPartnerships(pop, mixing_matrix)

  ## Calculate lambda
  calcLambda(pop, mixing_matrix, adjusted_partners)

  ## Transmit infections
  transmit(pop, lambda_mat)

  # Compute end-of-year population and set difference back to zero for next iteration of loop
  pop[, c("count", "diff") := list(count + diff, 0)]

  # Adjust population to match risk prevalence
  riskAdjust(pop)

  # Increment time step
  tt <- tt + 1

}

## Save results
## save(population, births, deaths, incidence, interventions, dis_dist, tstep, year_start, year_end, date, name, file = paste("output/", date, "/", name, ".RData", sep = ""))
save(population, deaths, incidence, tstep, year_start, year_end, date, name, file = paste("output/", date, "/", name, ".RData", sep = ""))

#
# ## Plot results
# source("plot_results.r")

