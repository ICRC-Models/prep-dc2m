rm(list = ls())

library(reshape2)
library(data.table)
library(ggplot2)
library(RColorBrewer)

load("/Volumes/udrive/models/r-models/2017-03-30/output/2017-04-03/nick_parameters.RData")

# 1) Population by age and sex and year and HIV status
# 2) Number of deaths by age and sex and year
# 3) Prevalence by age and sex and year
# 4) Number of new infections by age and sex and year
# 5) Incidence by age and sex and year
# 6) ART coverage by CD4 count and year (among HIV+)
# 7) Number of HIV+ people in each CD4 category, by sex and year
# 8) Number of HIV+ people in each VL category, by sex and year

## Formatting
population[, c("year", "year_exact", "model") := list(floor(year_start + (time - 1) * tstep), year_start + (time - 1) * tstep, "allen")] 
deaths[, c("year", "year_exact", "total_deaths", "model") := list(floor(year_start + (time - 1) * tstep), year_start + (time - 1) * tstep, back_deaths + hiv_deaths, "allen")] 
incidence[, c("year", "year_exact", "total_infections", "model") := list(floor(year_start + (time - 1) * tstep), year_start + (time - 1) * tstep, horiz_infections + vert_infections, "allen")] 
interventions[, c("year", "year_exact", "model") := list(floor(year_start + (time - 1) * tstep), year_start + (time - 1) * tstep, "allen")] 
dis_dist[, c("year", "year_exact", "model") := list(floor(year_start + (time - 1) * tstep), year_start + (time - 1) * tstep, "allen")]

population$male <- factor(population$male, levels = c(0, 1), labels = c("Female", "Male"))
population$hiv <- factor(population$hiv, levels = c(0, 1), labels = c("Negative", "Positive"))
population$age <- factor(population$age, levels = seq(1, 12), labels = c("0-4", "5-9", "10-14", "15-19", "20-24", "25-29", "30-34", "35-39", "40-44", "45-49", "50-54", "55-59"))


## Load Nick's data
nick_pop <- read.delim("/Volumes/ICRC/Studies/Combination Prevention/CP Models/Development/2017-04-10/nick/08-Apr-2017 NT_HIV_Results.dat", sep = ",", header = FALSE)
names(nick_pop) <- c("hiv", "age", "male", "year_exact", "cd4", "vl", "art", "pop_size")
nick_pop <- as.data.table(nick_pop)
nick_pop[, "model" := "nick"]
nick_pop$male <- factor(nick_pop$male, levels = c(0, 1), labels = c("Female", "Male"))
nick_pop$hiv <- factor(nick_pop$hiv, levels = c(0, 1), labels = c("Negative", "Positive"))
nick_pop$age <- factor(nick_pop$age, levels = seq(1, 12), labels = c("0-4", "5-9", "10-14", "15-19", "20-24", "25-29", "30-34", "35-39", "40-44", "45-49", "50-54", "55-59"))
  
## Formatting
## Age-specific population
age_pop <- population[, list(total = sum(pop_size)), by= list(age, male, year_exact, model)]
age_pop_nick <- nick_pop[, list(total = sum(pop_size)), by= list(age, male, year_exact, model)]

age_pop_combined <- rbind(age_pop, age_pop_nick)

age_pop_plot <- ggplot(data = age_pop_combined, aes(x = year_exact, y = total / 1000000)) +
  geom_line(aes(colour = model, linetype = male)) +
  scale_x_continuous(limits = c(year_start, year_end), breaks = seq(year_start, year_end, by = 10)) +
  xlab("Year") + ylab("Population (Millions)") +
  facet_wrap(~age) +
  ggtitle("Age-specific population")

## Prevalence
age_prev <- population[hiv == "Positive", list(hiv_total = sum(pop_size)), by = list(hiv, male, age, year_exact, model)]
setkey(age_prev, male, age, year_exact)
setkey(age_pop, male, age, year_exact)
age_prev[age_pop, prev := 100 * hiv_total/total]

age_prev_nick <- nick_pop[hiv == "Positive", list(hiv_total = sum(pop_size)), by = list(hiv, male, age, year_exact, model)]
setkey(age_prev_nick, male, age, year_exact)
setkey(age_pop_nick, male, age, year_exact)
age_prev_nick[age_pop_nick, prev := 100 * hiv_total/total]

age_prev_combined <- rbind(age_prev, age_prev_nick)
## Age-specific prevalence plot
age_prev_plot <- ggplot(data = age_prev_combined, aes(x = year_exact, y = prev)) +
  geom_line(aes(colour = model, linetype = male)) +
  scale_x_continuous(limits = c(year_start, year_end), breaks = seq(year_start, year_end, by = 10)) +
  xlab("Year") + ylab("HIV Prevalence (%)") +
  facet_wrap(~age) +
  ggtitle("Age-specific HIV prevalence")

## ART coverage
art_cd4 <- interventions[hiv == 1, list(size = sum(total)), by = list(cd4, year_exact, art, model)]
art_cd4 <- art_cd4[art == 1]
hiv_cd4_pop <- dis_dist[, list(denom = sum(total)), by = list(cd4, year_exact, model)]
setkey(art_cd4, cd4, year_exact)
setkey(hiv_cd4_pop, cd4, year_exact)
art_cd4[hiv_cd4_pop, cov := 100 * size / denom]
art_cd4[is.na(cov), cov := 0]

art_cd4_nick <- nick_pop[hiv == "Positive" & art == 1, list(size = sum(pop_size)),  by = list(cd4, year_exact, art, model)]
hiv_cd4_pop_nick <- nick_pop[hiv == "Positive", list(denom = sum(pop_size)), by = list(cd4, year_exact, model)]
setkey(art_cd4_nick, cd4, year_exact)
setkey(hiv_cd4_pop_nick, cd4, year_exact)
art_cd4_nick[hiv_cd4_pop_nick, cov := 100 * size / denom]
art_cd4_nick[is.na(cov), cov := 0]

## Disease distribution
cd4_stats <- dis_dist[cd4 > 0 & vl > 0, list(size = sum(total)), by = list(male, cd4, year_exact)]
vl_stats <- dis_dist[vl > 0 & cd4 > 0, list(size = sum(total)), by = list(male, cd4, year_exact)]

## Make plots
pdf(file = "/Volumes/ICRC/Studies/Combination Prevention/CP Models/Development/2017-04-10/output/plots.pdf", width = 10, height = 8)
print(age_pop_plot)
print(age_prev_plot)
dev.off()

