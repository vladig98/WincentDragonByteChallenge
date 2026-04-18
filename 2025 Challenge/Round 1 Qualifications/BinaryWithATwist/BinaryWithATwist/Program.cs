/*
 * CORE CONCEPT: POSITIONAL NOTATION
 * A number system is just a way to pack values into "slots."
 * In Base 10, slots are powers of 10: [10^2][10^1][10^0] (Hundreds, Tens, Ones).
 * In Base B, slots are powers of B: [B^2][B^1][B^0].
 * * Example: 123 in Base 10 is (1 * 10^2) + (2 * 10^1) + (3 * 10^0).
 * * PROPERTY 1: THE INVERSE RELATIONSHIP (BASE VS. DIGITS)
 * As the Base grows larger, the number of digits needed to represent N shrinks.
 * Base 2: 123 is "1111011" (7 digits)
 * Base 122: 123 is "11" (2 digits)
 * * PROPERTY 2: THE UPPER BOUND (N-1)
 * Every number N >= 2 can be written as "11" in Base (N-1).
 * Calculation: 1 * (N-1)^1 + 1 * (N-1)^0  =>  N-1 + 1 = N.
 * This guarantees a solution always exists.
 * * THE "TWO-PRONGED" OPTIMIZATION STRATEGY:
 * * PHASE 1: LINEAR SCAN (Small Bases, Many Digits)
 * We loop from Base 3 to 1,000,000. 
 * This covers cases where the number is "long" (e.g., "1010110").
 * We use simple division/remainder to check for 0s and 1s.
 * * PHASE 2: PATTERN SEARCH (Huge Bases, Few Digits)
 * Why 1,000,000? Because if the Base > 1,000,000, then Base^3 > 10^18.
 * Since the max input N is 10^18, any base larger than 1,000,000 
 * CANNOT have more than 3 digits (B^2 + B + 1). 
 * * Instead of checking millions of bases, we check the few possible patterns:
 * Patterns: "111", "110", "101", "100", and finally "11".
 * * Since a pattern (like B^2 + B + 1) always increases as B increases, 
 * we use Binary Search to find the exact B that satisfies the equation for N.
 */

string[] fileNames = ["B1.in", "B2.in", "B3.in"];
string[] fileNamesResults = ["B1_results.in", "B2_results.in", "B3_results.in"];

DirectoryInfo? dir = Directory.GetParent(Directory.GetCurrentDirectory());
string rootFolder = dir?.Parent?.Parent?.Parent?.FullName ?? string.Empty;

int length = fileNames.Length;
for (int fileNameIndex = 0; fileNameIndex < length; fileNameIndex++)
{
    string path = Path.Combine(rootFolder, fileNames[fileNameIndex]);
    string[] inputNumbers = await File.ReadAllLinesAsync(path);
    int testCases = int.Parse(inputNumbers[0]);

    StringBuilder results = new();
    for (int testCaseIndex = 1; testCaseIndex <= testCases; testCaseIndex++)
    {
        long number = long.Parse(inputNumbers[testCaseIndex]);
        if (number < 0 || number == 2)
        {
            results.AppendLine("-1");
            continue;
        }

        if (number == 0 || number == 1)
        {
            results.AppendLine("3");
            continue;
        }

        long lowerBound = 3;
        long upperBound = 1_000_000;

        bool isBaseValid = false;
        for (long i = lowerBound; i <= upperBound; i++)
        {
            isBaseValid = IsNumberToBaseZerosAndOnesOnly(number, i);
            if (isBaseValid)
            {
                results.AppendLine(i.ToString());
                break;
            }
        }

        if (isBaseValid)
        {
            continue;
        }

        Func<long, BigInteger>[] getNumberFunctions =
        [
            // 111 - 1 * base^2 + 1 * base^1 + 1 * base^0
            // 110 - 1 * base^2 + 1 * base^1 + 0 * base^0
            // 101 - 1 * base^2 + 0 * base^1 + 1 * base^0
            // 100 - 1 * base^2 + 0 * base^1 + 0 * base^0
            (@base) => BigInteger.Add(BigInteger.Add(BigInteger.Multiply(@base, @base), @base), 1),
            (@base) => BigInteger.Add(BigInteger.Multiply(@base, @base), @base),
            (@base) => BigInteger.Add(BigInteger.Multiply(@base, @base), 1),
            (@base) => BigInteger.Multiply(@base, @base)
        ];

        bool found = false;
        for (int i = 0; i < getNumberFunctions.Length; i++)
        {
            Func<long, BigInteger> currentFunction = getNumberFunctions[i];

            lowerBound = 1_000_000;
            upperBound = number - 1;

            long middle = BinarySearch(lowerBound, upperBound);
            BigInteger converted = currentFunction(middle);

            found = converted == number;
            while (lowerBound <= upperBound)
            {
                if (found)
                {
                    break;
                }

                if (converted > number)
                {
                    upperBound = middle - 1;
                }
                else
                {
                    lowerBound = middle + 1;
                }

                middle = BinarySearch(lowerBound, upperBound);
                converted = currentFunction(middle);
                found = converted == number;
            }

            if (found)
            {
                results.AppendLine(middle.ToString());
                break;
            }
        }

        if (!found)
        {
            results.AppendLine((number - 1).ToString());
        }
    }

    string outputPath = Path.Combine(rootFolder, fileNamesResults[fileNameIndex]);
    await File.WriteAllTextAsync(outputPath, results.ToString());
}

static bool IsNumberToBaseZerosAndOnesOnly(long number, long @base)
{
    while (number > 0)
    {
        if (number % @base >= 2)
        {
            return false;
        }

        number /= @base;
    }

    return true;
}

static long BinarySearch(long lowerBound, long upperBound)
{
    return (lowerBound + upperBound) / 2;
}