string[] inputFiles = ["M0_example.in", "M1.in", "M2.in", "M3.in", "M4.in", "M5.in"];
string[] outputFiles = ["M0_example.out", "M1.out", "M2.out", "M3.out", "M4.out", "M5.out"];

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

DirectoryInfo? parentFolder = directory.Parent
    ?? throw new DirectoryNotFoundException("Found the solution file, but it is at the root directory. Cannot go one folder up.");

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

            writer.WriteLine();
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}