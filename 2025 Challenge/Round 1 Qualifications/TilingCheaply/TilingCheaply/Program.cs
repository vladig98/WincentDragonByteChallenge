using System.Text;

const string no = "NO";
const string yes = "YES";

string[] fileNames = ["T1.in", "T2.in", "T3.in"];
string[] fileNamesResults = ["T1_results.in", "T2_results.in", "T3_results.in"];

int length = fileNames.Length;

for (int fileNameIndex = 0; fileNameIndex < length; fileNameIndex++)
{
    string[] inputNumbers = await File.ReadAllLinesAsync($"../../../../{fileNames[fileNameIndex]}");
    byte testCases = byte.Parse(inputNumbers[0]);

    List<string> results = [];

    for (int i = 1; i <= testCases; i++)
    {
        int dimension = int.Parse(inputNumbers[i]);

        if (dimension < 2 || (dimension < 11 && dimension % 2 != 0))
        {
            results.Add(no);

            continue;
        }

        results.Add(yes);
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

                results.Add(sb.ToString());
                results.Add(sb.ToString());

                if (dimension / 2 % 2 == 0)
                {
                    value = -value;
                }
            }
        }
    }

    await File.WriteAllLinesAsync($"../../../../{fileNamesResults[fileNameIndex]}", results);
}