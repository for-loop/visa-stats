# Table of Contents
1. [Problem](README.md#problem)
2. [Approach](README.md#approach)
3. [Run](README.md#run)

# Problem

This is my submission to the Insight Data Engineering coding challenge. See the details [here](https://github.com/InsightDataScience/h1b_statistics).

**Objective:** Build an algorithm that reads an input `csv` file and outputs two `txt` files, each summarizing stats on certified visa applications.

Some of the problems to consider include the following:

* The content of the input file may vary, e.g., total number of applicants, column number, column order, etc..
* The content of the output files must conform to the [specified format](https://github.com/InsightDataScience/h1b_statistics/blob/master/README.md#output).
* External libraries (e.g, Pandas for Python) cannot be used. (Standard libraries, however, are permitted)
* Only one test case has been provided. Additional test cases should be built.
* Repository directory must conform to the [specified structure](https://github.com/InsightDataScience/h1b_statistics/blob/master/README.md#tips-on-getting-an-interview)
* The `run.sh` script should be properly configured to compile and execute the program.

# Approach

1. Open the input file
2. Read the first line, which is assumed to contain the header. Parse each column name and store column number in a hash table.
3. Iterate through rest of the lines. For each line, parse columns to first check if the application was certified. If certified, then parse columns from the beginning to the columns of interest and increment two map objects, where key is either occupation or state* and value is the count. Certification is checked in a separate while-loop since the order of the column is undefined.
4. Close the input file.
5. Write summary stats to the output files. The summary will be ordered first by the highest number of certification, then alphabetically by name for ties, if any. The format of the percentage conforms to the [specification](https://github.com/InsightDataScience/h1b_statistics/blob/master/README.md#output). The summary will be capped to the first 10.
6. Close the output files.

*When more than one worksite state is given, only the first (e.g., "LCA_CASE_WORKLOC1_STATE" for 2014) will be considered.

I coded in **C++11**.

**Test cases**
* test_1: A test case provided by Insight
* test_2: Same as test_1 except "CASE_STATUS" column has been moved to the far right
* test_3: Same as test_1 except "SOC_NAME" column has been moved to the far right
* test_4: First 50 rows of `H1B_FY_2014.csv`
* test_5: First 50 rows of `H1B_FY_2015.csv`
* test_6: First 50 rows of `H1B_FY_2016.csv`
* test_7: Randomly selected 10% of `H1B_FY_2014.csv` (51,951 rows)
* test_8: Randomly selected 10% of `H1B_FY_2015.csv` (61,881 rows)
* test_9: Randomly selected 20% of `H1B_FY_2016.csv` (129,571 rows)

# Run

1. From the Terminal, cd to the directory containing `run.sh`
2. Make sure `./input/h1b_input.csv` is present
3. Execute the following command:
```bash
./run.sh
```
4. Open and review `./output/top_10_occupations.txt` and `./output/top_10_states.txt`
