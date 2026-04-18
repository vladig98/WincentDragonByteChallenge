using System.Text;

string[] inputs = ["D1.in", "D2.in", "D3.in", "D4.in"];
string[] outputs = ["D1.out", "D2.out", "D3.out", "D4.out"];
const string parent = "..";
const string digitsFile = "D_digits.txt";
string path = Path.Combine(parent, parent, parent, parent, parent);

string[] digitsFileContent = await File.ReadAllLinesAsync(Path.Combine(path, digitsFile));
Dictionary<string, int> digits = [];
int value = 0;
StringBuilder sb = new StringBuilder();

foreach (string digitRow in digitsFileContent)
{
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

int filesLength = inputs.Length;

for (int i = 0; i < filesLength; i++)
{
    string[] fileContent = await File.ReadAllLinesAsync(Path.Combine(path, inputs[i]));
    long testCases = long.Parse(fileContent[0]);
    List<int> results = [];

    for (int j = 1; j <= testCases; j++)
    {
        sb.Clear();

        string[] dimensions = fileContent[j].Split(' ');
        long rows = long.Parse(dimensions[0]);
        long cows = long.Parse(dimensions[1]);

        for (int row = 0; row < rows; row++)
        {
            j++;
            sb.AppendLine(fileContent[j]);
        }

        string digit = sb.ToString().TrimEnd(Environment.NewLine.ToCharArray());
        if (digits.TryGetValue(digit, out int digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string digit90 = RotateDigit(digit);

        if (digits.TryGetValue(digit90, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string digit180 = RotateDigit(digit90);

        if (digits.TryGetValue(digit180, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string digit270 = RotateDigit(digit180);

        if (digits.TryGetValue(digit270, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string flipped = FlipDigit(digit);

        if (digits.TryGetValue(flipped, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string flipped90 = FlipDigit(digit90);

        if (digits.TryGetValue(flipped90, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string flipped180 = FlipDigit(digit180);

        if (digits.TryGetValue(flipped180, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string flipped270 = FlipDigit(digit270);

        if (digits.TryGetValue(flipped270, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string digitNonDistorted = RemoveRowDistortion(digit, rows);

        if (digits.TryGetValue(digitNonDistorted, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string digit90NonDistorted = RemoveRowDistortion(digit90, rows);

        if (digits.TryGetValue(digit90NonDistorted, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string digit180NonDistorted = RemoveRowDistortion(digit180, rows);

        if (digits.TryGetValue(digit180NonDistorted, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string digit270NonDistorted = RemoveRowDistortion(digit270, rows);

        if (digits.TryGetValue(digit270NonDistorted, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string flippedNonDistorted = RemoveRowDistortion(flipped, rows);

        if (digits.TryGetValue(flippedNonDistorted, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string flipped90NonDistorted = RemoveRowDistortion(flipped90, rows);

        if (digits.TryGetValue(flipped90NonDistorted, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string flipped180NonDistorted = RemoveRowDistortion(flipped180, rows);

        if (digits.TryGetValue(flipped180NonDistorted, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }

        string flipped270NonDistorted = RemoveRowDistortion(flipped270, rows);

        if (digits.TryGetValue(flipped270NonDistorted, out digitAsInt))
        {
            results.Add(digitAsInt);
            continue;
        }
    }

    await File.WriteAllLinesAsync(Path.Combine(path, outputs[i]), results.Select(x => x.ToString()));
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