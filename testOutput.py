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
                self.assertAlmostEqual(float(val1), float(val2), places = 7, msg = "row %i"%row)

    def test_distributeCondoms(self):
        lines1, lines2 = loadCSVs("distributeCondoms.out", "distributeCondoms.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 7, msg = "row %i"%row)

    def test_addBirths(self):
        lines1, lines2 = loadCSVs("addBirths.out", "addBirths.cout")
        row = 0;
        for row1, row2 in zip(lines1, lines2):
            row += 1;
            # print(row)
            for val1, val2 in zip(row1.split(","), row2.split(",")):
                self.assertAlmostEqual(float(val1), float(val2), places = 7, msg = "row %i"%row)



if __name__ == '__main__':
    unittest.main()