import multiprocessing
import itertools

nSteps = 410
epsilon = 0.0000001

fileBaseNames = [
    "distributeART",
    "distributeCondoms",
    "addBirths",
    "subtractDeaths",
    "agePop",
    "progressDisease",
    "mixing_matrix",
    "lambda_mat",
    "adjusted_partners",
    "transmit",
    "endPop",
    "riskAdjust"
]

def doSequence(fileBaseName):
    for i in range(nSteps):
        baseWithInt = fileBaseName + "_%i" % i
        f1 = open("rout/" + baseWithInt + ".out")
        f2 = open("cout/" + baseWithInt + ".cout")
        row = 0
        for line1, line2 in itertools.izip(f1, f2):
            row += 1
            for val1, val2 in zip(line1.split(","), line2.split(",")):
                if abs(float(val1) - float(val2) > epsilon):
                    f1.close()
                    f2.close()
                    return "%s failed at time %i on row: %i" % (baseWithInt, i, row)

        f1.close()
        f2.close()
    return "%s passed at all time steps" % baseWithInt


if __name__ == "__main__":
    p = multiprocessing.Pool(12)
    output = p.map(doSequence, fileBaseNames)
    for line in output:
        print line