string[] fileNames = ["B1.in", "B2.in", "B3.in"];
string[] fileNamesResults = ["B1_results.in", "B2_results.in", "B3_results.in"];

int length = fileNames.Length;

for (int fileNameIndex = 0; fileNameIndex < length; fileNameIndex++)
{
    string[] inputNumbers = await File.ReadAllLinesAsync($"../../../../{fileNames[fileNameIndex]}");
    int testCases = int.Parse(inputNumbers[0]);

    List<long> results = [];

    for (int i = 1; i <= testCases; i++)
    {
        long number = long.Parse(inputNumbers[i]);
        long winningBase = BaseConverter.FindBase(number);

        results.Add(winningBase);
    }

    await File.WriteAllLinesAsync($"../../../../{fileNamesResults[fileNameIndex]}", results.Select(x => x.ToString()));
}