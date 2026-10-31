int[] inputs = [4, 6, 8, 9];
string[] outputFiles = ["G1.out", "G2.out", "G3.out", "G4.out"];

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

for (int f = 0; f < outputFiles.Length; f++)
{
    int n = inputs[f];
    string outFilePath = Path.Combine(outputPath, outputFiles[f]);

    using (StreamWriter writer = new(outFilePath))
    {
        writer.WriteLine();
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}