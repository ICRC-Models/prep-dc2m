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
        lines1, lines2 = loadCSVs("mixing_matrix.out", "mixing_matrix.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)         

    def test_adjusted_partners(self):
        lines1, lines2 = loadCSVs("adjusted_partners.out", "adjusted_partners.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)  

    def test_lambda_mat(self):
        lines1, lines2 = loadCSVs("lambda_mat.out", "lambda_mat.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 15, msg = "row %i"%row)

    def test_transmit(self):
        lines1, lines2 = loadCSVs("transmit.out", "transmit.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 10, msg = "row %i"%row)

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



if __name__ == '__main__':
    unittest.main()
