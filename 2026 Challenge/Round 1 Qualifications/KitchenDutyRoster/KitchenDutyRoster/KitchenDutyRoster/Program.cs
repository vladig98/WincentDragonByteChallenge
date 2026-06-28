using System.Numerics;

string[] inputFiles = ["K0_example.in", "K1.in", "K2.in", "K3.in", "K4.in", "K5.in"];
string[] outputFiles = ["K0_example.out", "K1.out", "K2.out", "K3.out", "K4.out", "K5.out"];

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

            string[] inputTokens = caseData.Split(' ');
            int n = int.Parse(inputTokens[0]);
            int k = int.Parse(inputTokens[1]);
            int p = int.Parse(inputTokens[2]);

            BigInteger days = n < 2 * k ? Choose(n, k) : Choose(n - 1, k - 1);
            writer.WriteLine(days);

            if (p == 1)
            {
                if (n < 2 * k)
                {
                    PrintCombinations(writer, n, k);
                }
                else
                {
                    PrintCombinations(writer, n, k, 0);
                }
            }
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}

static void PrintCombinations(StreamWriter writer, int n, int k, int fixedElement = -1)
{
    int targetK = fixedElement >= 0 ? k - 1 : k;
    int targetN = fixedElement >= 0 ? n - 1 : n;
    int startOffset = fixedElement >= 0 ? 1 : 0;

    if (targetK == 0)
    {
        if (fixedElement >= 0)
        {
            writer.WriteLine(fixedElement);
        }

        return;
    }

    int[] indices = new int[targetK];
    for (int i = 0; i < targetK; i++)
    {
        indices[i] = i;
    }

    int[] outputRow = new int[k];
    if (fixedElement >= 0)
    {
        outputRow[0] = fixedElement;
    }

    while (true)
    {
        int outIdx = fixedElement >= 0 ? 1 : 0;
        for (int i = 0; i < targetK; i++)
        {
            outputRow[outIdx] = indices[i] + startOffset;
            outIdx++;
        }

        writer.WriteLine(string.Join(" ", outputRow));

        int iBack = targetK - 1;
        while (iBack >= 0 && indices[iBack] == targetN - targetK + iBack)
        {
            iBack--;
        }

        if (iBack < 0)
        {
            break;
        }

        indices[iBack]++;
        for (int j = iBack + 1; j < targetK; j++)
        {
            indices[j] = indices[j - 1] + 1;
        }
    }
}

static BigInteger Choose(int n, int k)
{
    if (k < 0 || k > n)
    {
        return 0;
    }

    if (k == 0 || k == n)
    {
        return 1;
    }

    BigInteger num = 1;
    BigInteger den = 1;

    for (int i = 1; i <= k; i++)
    {
        num *= (n - i + 1);
        den *= i;
    }

    return num / den;
}