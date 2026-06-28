string path = "";
string[] inputFiles = ["R1.in", "R2.in"];
string[] outputFiles = ["R1.out", "R2.out"];

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

        Dictionary<char, int> deck = [];
        for (int i = 'A'; i <= 'Z'; i++)
        {
            deck.Add((char)i, i - 'A');
        }
        for (int i = 'a'; i <= 'z'; i++)
        {
            deck.Add((char)i, i - 'a' + 26);
        }

        int testcases = int.Parse(headerLine);
        for (int t = 0; t < testcases; t++)
        {
            List<int> breaksPerCase = [];
            for (int r = 0; r < 1_000; r++)
            {
                string? caseData = reader.ReadLine();
                if (caseData == null)
                {
                    break;
                }

                int breaks = 0;

                int[] positions = new int[52];
                for (int i = 0; i < caseData.Length; i++)
                {
                    int cardValue = deck[caseData[i]];
                    positions[cardValue] = i;
                }

                for (int v = 0; v < 51; v++)
                {
                    if (positions[v] > positions[v + 1])
                    {
                        breaks++;
                    }
                }

                breaksPerCase.Add(breaks);
            }

            double average = breaksPerCase.Average();
            string result = average < 25 ? "riffle" : "uniform";

            writer.WriteLine(result);
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}