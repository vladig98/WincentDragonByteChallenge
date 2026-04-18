const string parentDir = "..";
string path = Path.Combine(parentDir, parentDir, parentDir, parentDir, parentDir);
string[] fileContent = await File.ReadAllLinesAsync(Path.Combine(path, "H1.in"));

int numberOfTestCases = int.Parse(fileContent[0]);

for (int i = 1; i <= numberOfTestCases; i++)
{
    long numberOfNodes = long.Parse(fileContent[i]);

}

Console.WriteLine();