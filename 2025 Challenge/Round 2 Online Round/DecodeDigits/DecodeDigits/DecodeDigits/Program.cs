using System.Text;

const string digitsFile = "D_digits.txt";
string[] inputFiles = ["D0_example.in", "D1.in", "D2.in", "D3.in", "D4.in"];
string[] outputFiles = ["D0_example.out", "D1.out", "D2.out", "D3.out", "D4.out"];

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

    Dictionary<string, int> digits = [];
    StringBuilder sb = new();

    int value = 0;

    string digitsPath = Path.Combine(inputPath, digitsFile);
    using (StreamReader reader = new(digitsPath))
    {
        string? digitRow = reader.ReadLine();
        if (digitRow == null)
        {
            continue;
        }

        if (string.IsNullOrEmpty(digitRow))
        {
            digits.Add(sb.ToString().TrimEnd(Environment.NewLine.ToCharArray()), value);
            sb.Clear();
            value++;

            continue;
        }

        sb.AppendLine(digitRow);
    }

    if (sb.Length > 0)
    {
        digits.Add(sb.ToString().TrimEnd('\r', '\n'), value);
    }

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

            string[] dimensions = caseData.Split(' ');
            long rows = long.Parse(dimensions[0]);
            long cows = long.Parse(dimensions[1]);

            for (int row = 0; row < rows; row++)
            {
                caseData = reader.ReadLine();
                if (caseData == null)
                {
                    break;
                }

                sb.AppendLine(caseData);
            }

            string digit = sb.ToString().TrimEnd(Environment.NewLine.ToCharArray());
            if (digits.TryGetValue(digit, out int digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string digit90 = RotateDigit(digit);

            if (digits.TryGetValue(digit90, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string digit180 = RotateDigit(digit90);

            if (digits.TryGetValue(digit180, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string digit270 = RotateDigit(digit180);

            if (digits.TryGetValue(digit270, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string flipped = FlipDigit(digit);

            if (digits.TryGetValue(flipped, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string flipped90 = FlipDigit(digit90);

            if (digits.TryGetValue(flipped90, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string flipped180 = FlipDigit(digit180);

            if (digits.TryGetValue(flipped180, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string flipped270 = FlipDigit(digit270);

            if (digits.TryGetValue(flipped270, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string digitNonDistorted = RemoveRowDistortion(digit, rows);

            if (digits.TryGetValue(digitNonDistorted, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string digit90NonDistorted = RemoveRowDistortion(digit90, rows);

            if (digits.TryGetValue(digit90NonDistorted, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string digit180NonDistorted = RemoveRowDistortion(digit180, rows);

            if (digits.TryGetValue(digit180NonDistorted, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string digit270NonDistorted = RemoveRowDistortion(digit270, rows);

            if (digits.TryGetValue(digit270NonDistorted, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string flippedNonDistorted = RemoveRowDistortion(flipped, rows);

            if (digits.TryGetValue(flippedNonDistorted, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string flipped90NonDistorted = RemoveRowDistortion(flipped90, rows);

            if (digits.TryGetValue(flipped90NonDistorted, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string flipped180NonDistorted = RemoveRowDistortion(flipped180, rows);

            if (digits.TryGetValue(flipped180NonDistorted, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }

            string flipped270NonDistorted = RemoveRowDistortion(flipped270, rows);

            if (digits.TryGetValue(flipped270NonDistorted, out digitAsInt))
            {
                writer.WriteLine(digitAsInt);
                continue;
            }
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}

static string FlipDigit(string digit)
{
    StringBuilder sb = new StringBuilder();
    string[] rows = digit.Split(Environment.NewLine);
    int length = rows.Length;

    for (int i = 0; i < length; i++)
    {
        string row = rows[i];
        int rowLength = row.Length;

        for (int j = rowLength - 1; j >= 0; j--)
        {
            sb.Append(row[j]);
        }

        sb.AppendLine();
    }

    return sb.ToString().TrimEnd(Environment.NewLine.ToCharArray());
}

static string RotateDigit(string digit)
{
    StringBuilder sb = new StringBuilder();
    string[] rows = digit.Split(Environment.NewLine);
    int length = rows.Length;
    int colLength = rows[0].Length;

    for (int i = 0; i < colLength; i++)
    {
        for (int j = 0; j < length; j++)
        {
            sb.Append(rows[length - j - 1][i]);
        }

        sb.AppendLine();
    }

    return sb.ToString().TrimEnd(Environment.NewLine.ToCharArray());
}

static string RemoveRowDistortion(string digit, long rowsCount)
{
    StringBuilder sb = new StringBuilder();
    string[] rows = digit.Split(Environment.NewLine);
    int length = rows.Length - 1;
    int remainingLines = rows.Length - 15;

    // Do not remove distortion if it's rotated 90 / 270 degrees
    if (rows.Length != rowsCount)
    {
        return digit;
    }

    sb.AppendLine(rows[0]);

    for (int i = 1; i < length; i++)
    {
        // Skip removing rows if we've hit 15 (min length)
        string currentRow = rows[i];
        if (remainingLines == 0)
        {
            sb.AppendLine(currentRow);
            continue;
        }

        // Ignore dots
        if (!currentRow.Contains('#'))
        {
            sb.AppendLine(currentRow);
            continue;
        }

        string rowAbove = rows[i - 1];
        string rowBelow = rows[i + 1];

        int rowLength = currentRow.Length;
        bool distorted = true;

        for (int j = 0; j < rowLength; j++)
        {
            char currentChar = currentRow[j];
            char charAbove = rowAbove[j];
            char charBelow = rowBelow[j];

            // No distortion
            if (currentChar != charAbove && currentChar != charBelow)
            {
                sb.AppendLine(currentRow);
                distorted = false;

                break;
            }
        }

        if (distorted)
        {
            remainingLines--;
        }

        // Dom't add distorted rows to SB
    }

    sb.AppendLine(rows[^1]);

    return sb.ToString().TrimEnd(Environment.NewLine.ToCharArray());
}