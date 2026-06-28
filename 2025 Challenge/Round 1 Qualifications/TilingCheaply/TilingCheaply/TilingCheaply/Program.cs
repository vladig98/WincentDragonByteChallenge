using System.Text;

string[] inputFiles = ["T0_example.in", "T1.in", "T2.in", "T3.in"];
string[] outputFiles = ["T0_example.out", "T1.out", "T2.out", "T3.out"];

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

        const string no = "NO";
        const string yes = "YES";

        int testcases = int.Parse(headerLine);
        for (int t = 0; t < testcases; t++)
        {
            string? caseData = reader.ReadLine();
            if (caseData == null)
            {
                break;
            }

            int dimension = int.Parse(caseData);

            if (dimension < 2 || (dimension < 11 && dimension % 2 != 0))
            {
                writer.WriteLine(no);

                continue;
            }

            writer.WriteLine(yes);
            StringBuilder sb = new();

            if (dimension % 2 == 0)
            {
                int value = 1;
                for (int j = 0; j < dimension; j += 2)
                {
                    sb.Clear();

                    for (int k = 0; k < dimension; k += 2)
                    {
                        if (value > 0)
                        {
                            sb.Append("aa");
                        }
                        else
                        {
                            sb.Append("bb");
                        }

                        value = -value;
                    }

                    writer.WriteLine(sb);
                    writer.WriteLine(sb);

                    if (dimension / 2 % 2 == 0)
                    {
                        value = -value;
                    }
                }
            }
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}