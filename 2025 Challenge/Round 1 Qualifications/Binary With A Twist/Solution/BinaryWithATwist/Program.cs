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

using System.Numerics;

string[] inputFiles = ["B0_example.in", "B1.in", "B2.in", "B3.in"];
string[] outputFiles = ["B0_example.out", "B1.out", "B2.out", "B3.out"];

string currentPath = Directory.GetCurrentDirectory();
DirectoryInfo? directory = new(currentPath);

while (directory is not null && !directory.EnumerateFiles().Any(f =>
    f.Extension.Equals(".sln", StringComparison.OrdinalIgnoreCase) ||
    f.Extension.Equals(".slnx", StringComparison.OrdinalIgnoreCase)))
{
    directory = directory.Parent;
}

if (directory is null)
{
    throw new FileNotFoundException("Could not find a .sln or .slnx file in the directory hierarchy.");
}

DirectoryInfo? parentFolder = directory.Parent;
if (parentFolder is null)
{
    throw new DirectoryNotFoundException("Found the solution file, but it is at the root directory. Cannot go one folder up.");
}

string inputPath = Path.Combine(parentFolder.FullName, "Inputs");
string outputPath = Path.Combine(parentFolder.FullName, "Outputs");

for (int f = 0; f < inputFiles.Length; f++)
{
    string inFilePath = Path.Combine(inputPath, inputFiles[f]);
    string outFilePath = Path.Combine(outputPath, outputFiles[f]);

    Console.WriteLine($"Processing: {inputFiles[f]}...");

    using (StreamReader reader = new(inFilePath))
    using (StreamWriter writer = new(outFilePath))
    {
        string? headerLine = reader.ReadLine();
        while (string.IsNullOrWhiteSpace(headerLine) && headerLine != null)
        {
            headerLine = reader.ReadLine();
        }

        if (headerLine == null)
        {
            continue;
        }

        int testcases = int.Parse(headerLine);
        for (int t = 0; t < testcases; t++)
        {
            string? caseData = reader.ReadLine();
            if (caseData == null)
            {
                break;
            }

            long number = long.Parse(caseData);
            if (number < 0 || number == 2)
            {
                writer.WriteLine("-1");
                continue;
            }

            if (number == 0 || number == 1)
            {
                writer.WriteLine("3");
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
                    writer.WriteLine(i);
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
                    writer.WriteLine(middle);
                    break;
                }
            }

            if (!found)
            {
                writer.WriteLine(number - 1);
            }
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
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