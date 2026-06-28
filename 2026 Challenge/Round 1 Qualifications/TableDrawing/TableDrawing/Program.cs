string path = "";
string[] inputFiles = ["T1.in"];
string[] outputFiles = ["T1.out"];

for (int f = 0; f < inputFiles.Length; f++)
{
    string inFilePath = Path.Combine(path, inputFiles[f]);
    string outFilePath = Path.Combine(path, outputFiles[f]);

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