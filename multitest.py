import multiprocessing
import itertools
import glob
import os

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


    fileList = glob.glob('./cout/*.cout')


    if fileList:
        used_numbers = []
        for f in fileList:
            try:
                file_number = int(os.path.splitext(os.path.basename(f))[0].split('_')[-1])
                used_numbers.append(file_number)
            except ValueError:
                pass
        nSteps = max(used_numbers) + 1
    else:
        return "Failed: no files in ./cout found"

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
    ncores = multiprocessing.cpu_count() - 1
    p = multiprocessing.Pool(ncores)
    output = p.map(doSequence, fileBaseNames)
    for line in output:
        print line