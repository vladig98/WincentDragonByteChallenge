using System.Text;

string[] inputFiles = ["C0_example.in", "C1.in", "C2.in", "C3.in"];
string[] outputFiles = ["C0_example.out", "C1.out", "C2.out", "C3.out"];

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

const string no = "NO";
const string yes = "YES";

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
            int numberOfPlayer = int.Parse(tokens[0]);
            int numberOfDraws = int.Parse(tokens[1]);

            char[,] tournament = new char[numberOfPlayer, numberOfPlayer];
            for (int i = 0; i < numberOfPlayer; i++)
            {
                for (int j = 0; j < numberOfPlayer; j++)
                {
                    if (i == j)
                    {
                        tournament[i, j] = 'X';
                        //writer.Write('X');
                        continue;
                    }

                    if (tournament[i, j] != default)
                    {
                        continue;
                    }

                    if (numberOfDraws > 0)
                    {
                        tournament[i, j] = 'D';
                        tournament[j, i] = 'D';
                        numberOfDraws--;
                    }
                }
            }

            double maxPoints = 0;
            bool noSolution = false;

            for (int i = numberOfPlayer - 1; i >= 0; i--)
            {
                double points = 0;
                for (int j = numberOfPlayer - 1; j >= 0; j--)
                {
                    if (i == j)
                    {
                        tournament[i, j] = 'X';
                        continue;
                    }

                    if (tournament[i, j] != default)
                    {
                        if (tournament[i, j] == 'D')
                        {
                            points += 0.5;
                        }
                        else if (tournament[i, j] == 'W')
                        {
                            points += 1;
                        }

                        continue;
                    }

                    tournament[i, j] = 'W';
                    tournament[j, i] = 'L';
                    points += 1;
                }

                //writer.WriteLine();

                if (points > maxPoints)
                {
                    maxPoints = points;
                }
                else if (points == maxPoints)
                {
                    noSolution = true;
                    break;
                }
            }

            if (noSolution)
            {
                writer.WriteLine(no);
                continue;
            }
            else
            {
                writer.WriteLine(yes);
            }

            for (int i = 0; i < numberOfPlayer; i++)
            {
                for (int j = 0; j < numberOfPlayer; j++)
                {
                    writer.Write(tournament[i, j]);
                }

                writer.WriteLine();
            }
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}