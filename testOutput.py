import numpy
import unittest

def loadCSVs(file1, file2):
    with open(file1, "r") as f:
        f1lines = f.readlines()
    with open(file2, "r") as f:
        f2lines = f.readlines()
    return f1lines, f2lines

class TestCSVs(unittest.TestCase):

    def test_distributeART(self):
        lines1, lines2 = loadCSVs("distributeART.out", "distributeART.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

    def test_distributeCondoms(self):
        lines1, lines2 = loadCSVs("distributeCondoms.out", "distributeCondoms.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

    def test_addBirths(self):
        lines1, lines2 = loadCSVs("addBirths.out", "addBirths.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

    def test_subtractDeaths(self):
        lines1, lines2 = loadCSVs("subtractDeaths.out", "subtractDeaths.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

    def test_agePop(self):
        lines1, lines2 = loadCSVs("agePop.out", "agePop.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

    def test_progressDisease(self):
        lines1, lines2 = loadCSVs("progressDisease.out", "progressDisease.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

    def test_mixing_matrix(self):
        lines1, lines2 = loadCSVs("mixing_matrix_1.out", "mixing_matrix_test.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

    def test_adjusted_partners(self):
        lines1, lines2 = loadCSVs("adjusted_partners_1.out", "adjusted_partners_test.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

    def test_lambda_mat(self):
        lines1, lines2 = loadCSVs("lambda_mat_1.out", "lambda_mat_test.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

    def test_transmit(self):
        lines1, lines2 = loadCSVs("transmit_1.out", "transmit_test.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 9, msg = "row %i"%row)

    def test_endPop(self):
        lines1, lines2 = loadCSVs("endPop.out", "endPop.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 9, msg = "row %i"%row)

    def test_riskAdjust(self):
        lines1, lines2 = loadCSVs("riskAdjust.out", "riskAdjust.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 8, msg = "row %i"%row)

    def test_outputPop(self):
        lines1, lines2 = loadCSVs("pop_final.out", "pop_final.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 8, msg = "row %i"%row)


    def test_allOutput(self):
        filenum = 0
        for ii in range(410):
            filenum += 1
            rfile = "pop_%i.out"%ii
            cfile = "pop_%i.cout"%ii
            lines1, lines2 = loadCSVs(rfile, cfile)
            row = 0
            for row1, row2 in zip(lines1, lines2):
                row += 1;
                # print(row)
                column = 0
                for val1, val2 in zip(row1.split(","), row2.split(",")):
                    column += 1
                    self.assertAlmostEqual(float(val1), float(val2), places = 8, msg = "file %s row %i column %i"%(rfile, row, column))

    def test_final(self):
        lines1, lines2 = loadCSVs("pop_409.out", "pop_final.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 8, msg = "row %i"%row)


    def generic_test(self, file1, file2, places):
        lines1, lines2 = loadCSVs(file1, file2)
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = places, msg = "%s: row %i"%(file1, row))

    def test_everything(self):
        fileBaseNames = ["distributeART", "distributeCondoms", "addBirths", "subtractDeaths", "agePop", "progressDisease", "mixing_matrix", "transmit", "endPop", "riskAdjust"]
        ## fileBaseNames = ["mixing_matrix", "transmit"]
        ## precisionPlaces = [7, 7]
        precisionPlaces = [7, 7, 7, 7, 7, 7, 7, 7, 7, 7] # one for each type of file?
        for timeStep in range(410):
            for fileBase, precision in zip(fileBaseNames,precisionPlaces):
                rFile = fileBase + "_" + str(timeStep) + ".out"
                cFile = fileBase + "_" + str(timeStep) + ".cout"
                self.generic_test(rFile, cFile, precision)

    def test_distributeART2(self):

        # fileBaseNames = ["mixing_matrix", "transmit"]
        ## precisionPlaces = [7, 7]
        precision = 7
        fileBase = "distributeART"
        timeStep = 0
        rFile = fileBase + "_" + str(timeStep) + ".out"
        cFile = fileBase + "_" + str(timeStep) + ".cout"
        self.generic_test(rFile, cFile, precision)

    def test_distributeCondoms2(self):

        ## fileBaseNames = ["mixing_matrix", "transmit"]
        ## precisionPlaces = [7, 7]
        precision = 7
        fileBase = "distributeCondoms"
        timeStep = 0
        rFile = fileBase + "_" + str(timeStep) + ".out"
        cFile = fileBase + "_" + str(timeStep) + ".cout"
        self.generic_test(rFile, cFile, precision)

    def test_addBirths2(self):
        ## fileBaseNames = ["mixing_matrix", "transmit"]
        ## precisionPlaces = [7, 7]
        precision = 7
        fileBase = "addBirths"
        timeStep = 0
        rFile = fileBase + "_" + str(timeStep) + ".out"
        cFile = fileBase + "_" + str(timeStep) + ".cout"
        self.generic_test(rFile, cFile, precision)

    def test_subtractDeaths2(self):
        ## fileBaseNames = ["mixing_matrix", "transmit"]
        ## precisionPlaces = [7, 7]
        precision = 7
        fileBase = "subtractDeaths"
        timeStep = 0
        rFile = fileBase + "_" + str(timeStep) + ".out"
        cFile = fileBase + "_" + str(timeStep) + ".cout"
        self.generic_test(rFile, cFile, precision)

    def test_agePop2(self):
        ## fileBaseNames = ["mixing_matrix", "transmit"]
        ## precisionPlaces = [7, 7]
        precision = 7
        fileBase = "agePop"
        timeStep = 0
        rFile = fileBase + "_" + str(timeStep) + ".out"
        cFile = fileBase + "_" + str(timeStep) + ".cout"
        self.generic_test(rFile, cFile, precision)

if __name__ == '__main__':
    unittest.main()

