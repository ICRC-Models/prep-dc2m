##################################################
## Allen Roberts
## July 27, 2015
## Description: Distributes HIV+ people onto ART to match expected coverage. Stratifies coverage by CD4 count
##################################################

# art_cov
# columns are cd4 1:5


# at the end of the day this function has ensured that
# the distribution of people with art 0:1 is according to props
# in pop: people (the count column) could have moved between consecutive
# pairs of rows:
# art column is 0 1 0 1 0 1 row-wise
# total count in matrix doesn't change
# and total count in art 0 1 pair doesnt change.

distributeART <- function(dt, time_index) {

  ## ART coverage by CD4 count

  coverage <- as.data.table(data.frame("cd4" = 1:5, prop =
    sapply(seq(1, length(art_cov)), function(x) {
      art_cov[[x]][time_index]
    })
  ))

  # print("pop")
  # print(dt)

  # print("time_index")
  # print(time_index)

  # coverage grabs the time_index timestep for 1:5 cd4
  # art_cov should be shape 5 x nTimeSteps

  # print("coverage")
  # print(coverage)
  # lapply x=0:1
  props <- rbindlist(lapply(0:1, function(x, d) data.table(d, art = x), d = coverage))


  # print("probps")
  # print(props)
  props[art == 0, prop := 1 - prop] # assigns a value to prop for art = 0 people
  # print("probps 2")
  # print(props)

  # props what proportion are on art and what prop are not on art
  # in props row 1 + 6 = 1, row 2 + 7 = 1  etc

  #proportion of population by cd4 count thats on art

  props$hiv <- 1 # this adds a column = 1 labeled hiv to make sure that this
  # function only operates on rows in pop for which hiv=1
  # print("probps 3")
  # print(props)
  setkey(props, hiv, cd4, art) # set key makes stuff go faster

  ## Sums the population across CD4 categories
  # print("dt before")
  # print(dt)
  dt[, c("art", "sum") := list(art, sum(count)), by = .(hiv, age, male, risk, cd4, vl, circ, prep, condom)]
  # summin the values in the count column by every unique combination of the things in .() (basically binning)
  # this goes into a new column called sum
  # print("dt after")
  # print(dt)
  ## Multiplies the summed population by the CD4 proportions
  setkey(dt, hiv, cd4, art)
  dt[props, count := sum * prop]
  # print("dt after props")
  # print(dt)

  dt[, sum := NULL]

  # this deletes sum column

}