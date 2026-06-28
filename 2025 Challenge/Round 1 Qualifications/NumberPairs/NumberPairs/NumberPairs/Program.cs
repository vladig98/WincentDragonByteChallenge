/// <summary>
///	We have a number d
///	We are looking for two number A and B so that A-B=d
///	The sum of the digits of A must be equal to sum of the digits of B, let's call it S for sum
///	We can represent every number as sum of powers of 10
///	Let's say we get the number 356, we can display it as 3 x 10^2 + 5 * 10^1 + 6 * 10^0 or 3x100 + 5*10 * 6*1
///	Each power of 10 can be represented as multiple of 9 + 1
///	100 => 9 * 11 + 1 | 99 + 1
///	10 => 9 + 1
///	1 => 0 * 9 + 1
///	Replacing the powers of 10 with the multiple of 9 representation, we get
///	3 x (99 + 1) + 5 * (9 + 1) + 6 * (0 + 1) = (297 + 3) + (45 + 5) + (0 + 6)
///	As we can see, we have a multiple of 9 plus the first digit, a multiple of nine plus the second digit and so on
///	If we subtract the seond number, the sum of digits will cancel out since they're equal
///	We will be left with a sum of two numbers multiple of 9
///	Summing two multiples of 9 will produce a multiple of 9 therefore 
///	The difference between two numbers with the same sum of their digits will be a multiple of nine
///	Since a number is multiple of 9, we can represent the number as 9 * k
///	We can assign the first number A to be that k value
///	And we can assign B to be 10 * k, so
///	A - B = k - 10k = |-9k| the number that we started with
///	Not only that, but since A and B differ by a factor of 10, B will have only one more digit than A
///	That digit will be 0 and the sum will not change, hence we will meet both requirments
/// </summary>

string[] inputFiles = ["N0_example.in", "N1.in", "N2.in", "N3.in"];
string[] outputFiles = ["N0_example.out", "N1.out", "N2.out", "N3.out"];

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

        const string none = "NONE";
        int testcases = int.Parse(headerLine);

        for (int t = 0; t < testcases; t++)
        {
            string? caseData = reader.ReadLine();
            if (caseData == null)
            {
                break;
            }

            long number = long.Parse(caseData);
            if (number % 9 != 0)
            {
                writer.WriteLine(none);
                continue;
            }

            long multiple = number / 9;

            long A = multiple;
            long B = multiple * 10;

            writer.WriteLine($"{A} {B}");
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}