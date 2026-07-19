using System.Text;

string[] inputFiles = ["B0_example.in", "B1.in", "B2.in", "B3.in", "B4.in", "B5.in"];
string[] outputFiles = ["B0_example.out", "B1.out", "B2.out", "B3.out", "B4.out", "B5.out"];

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

    StringBuilder sb = new();

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

            string[] tokens = caseData.Split(' ');
            int alphabetLength = int.Parse(tokens[0]);
            int costForHalfBit = int.Parse(tokens[1]);

            caseData = reader.ReadLine();
            if (caseData == null)
            {
                break;
            }

            tokens = caseData.Split(' ');
            long sum = 0;
            sb.Clear();

            for (int i = 0; i < tokens.Length; i++)
            {
                string token = tokens[i];
                sum += long.Parse(token);
                sb.Append(Convert.ToString(i, 2));
                sb.Append(' ');
            }

            writer.WriteLine(sum);
            writer.WriteLine(sb.ToString().TrimEnd());
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}