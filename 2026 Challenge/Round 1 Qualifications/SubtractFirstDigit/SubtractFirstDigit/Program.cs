using System.Numerics;

string path = "C:\\Users\\efbet\\Desktop\\Projects\\Misc\\wincent\\inputs_crlf";
string[] inputFiles = ["S1.in", "S2.in", "S3.in"];
string[] outputFiles = ["S1.out", "S2.out", "S3.out"];

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

            const int mod = 1_000_000_007;

            // Code here
            BigInteger counter = BigInteger.Zero;
            BigInteger number = BigInteger.Parse(caseData);

            int currentLength = caseData.Length;
            BigInteger divisor = BigInteger.Pow(10, currentLength - 1);

            while (number > 0)
            {
                if (number < 10)
                {
                    counter++;
                    break;
                }

                while (divisor > number)
                {
                    divisor /= 10;
                }

                int firstDigit = (int)(number / divisor);
                BigInteger temp = number % divisor;

                BigInteger steps = temp / firstDigit;
                BigInteger reminder = temp % firstDigit;

                if (reminder > 0)
                {
                    steps++;
                }
                else if (temp == 0)
                {
                    steps = 1;
                }

                counter += steps;
                number -= steps * firstDigit;
            }

            writer.WriteLine(counter % mod);
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}