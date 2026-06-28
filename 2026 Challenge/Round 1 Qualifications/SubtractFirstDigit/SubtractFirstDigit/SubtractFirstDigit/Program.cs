using System.Numerics;

string[] inputFiles = ["S0_example1.in", "S0_example2.in", "S1.in", "S2.in", "S3.in"];
string[] outputFiles = ["S0_example1.out", "S0_example2.out", "S1.out", "S2.out", "S3.out"];

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

            const int mod = 1_000_000_007;

            // Code here
            BigInteger counter = BigInteger.Zero;
            BigInteger number = BigInteger.Parse(caseData);

            int currentLength = caseData.Length;
            BigInteger divisor = BigInteger.Pow(10, currentLength - 1);

            while (number > 0)
            {
                if (number < 10)
                {
                    counter++;
                    break;
                }

                while (divisor > number)
                {
                    divisor /= 10;
                }

                int firstDigit = (int)(number / divisor);
                BigInteger temp = number % divisor;

                BigInteger steps = temp / firstDigit;
                BigInteger reminder = temp % firstDigit;

                if (reminder > 0)
                {
                    steps++;
                }
                else if (temp == 0)
                {
                    steps = 1;
                }

                counter += steps;
                number -= steps * firstDigit;
            }

            writer.WriteLine(counter % mod);
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}