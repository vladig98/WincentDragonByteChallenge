using System.Text;

string[] inputFiles = ["T0_example.in", "T1.in", "T2.in", "T3.in"];
string[] outputFiles = ["T0_example.out", "T1.out", "T2.out", "T3.out"];

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

        const string no = "NO";
        const string yes = "YES";

        int testcases = int.Parse(headerLine);
        for (int t = 0; t < testcases; t++)
        {
            string? caseData = reader.ReadLine();
            if (caseData == null)
            {
                break;
            }

            int dimensions = int.Parse(caseData);
            if (dimensions < 7 && dimensions % 2 != 0)
            {
                writer.WriteLine(no);
                continue;
            }

            writer.WriteLine(yes);
            char[,] matrix = new char[dimensions, dimensions];

            if (dimensions % 2 == 0)
            {
                PrintEven(writer, dimensions, matrix);
            }
            else
            {
                PrintOdd(writer, dimensions, matrix);
            }
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}

static void PrintOdd(StreamWriter writer, int dimensions, char[,] matrix)
{
    const int baseSolutionSize = 7;
    char letter = 'a';

    letter = PopulateBaseSolution(matrix, letter);

    for (int currentSize = baseSolutionSize; currentSize < dimensions; currentSize += 2)
    {
        letter = GenerateVerticalSlice(currentSize, letter, matrix);
        letter = GenerateHorizontalSlice(currentSize, letter, matrix);
        letter = Generate2x2Block(currentSize, currentSize, letter, matrix);
    }

    string result = GetResult(matrix);
    writer.Write(result);
}

static void PrintEven(StreamWriter writer, int dimensions, char[,] matrix)
{
    char letter = 'a';

    for (int rowIndex = 0; rowIndex < dimensions; rowIndex += 2)
    {
        for (int colIndex = 0; colIndex < dimensions; colIndex += 2)
        {
            letter = Generate2x2Block(rowIndex, colIndex, letter, matrix);
        }
    }

    string result = GetResult(matrix);
    writer.Write(result);
}

static string GetResult(char[,] matrix)
{
    StringBuilder sb = new();
    for (int i = 0; i < matrix.GetLength(0); i++)
    {
        for (int j = 0; j < matrix.GetLength(1); j++)
        {
            sb.Append(matrix[i, j]);
        }

        sb.AppendLine();
    }

    return sb.ToString();
}

static char PopulateBaseSolution(char[,] matrix, char letter)
{
    char currentLetter = letter;

    // O - Tetromino
    matrix[0, 0] = currentLetter;
    matrix[0, 1] = currentLetter;
    matrix[1, 0] = currentLetter;
    matrix[1, 1] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 1st Trimino
    matrix[0, 2] = currentLetter;
    matrix[0, 3] = currentLetter;
    matrix[1, 2] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 2nd Trimino
    matrix[0, 4] = currentLetter;
    matrix[0, 5] = currentLetter;
    matrix[1, 4] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 3rd Trimino
    matrix[0, 6] = currentLetter;
    matrix[1, 5] = currentLetter;
    matrix[1, 6] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 4th Trimino
    matrix[1, 3] = currentLetter;
    matrix[2, 3] = currentLetter;
    matrix[2, 4] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 5th Trimino
    matrix[2, 0] = currentLetter;
    matrix[2, 1] = currentLetter;
    matrix[3, 0] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 6th Trimino
    matrix[2, 2] = currentLetter;
    matrix[3, 2] = currentLetter;
    matrix[3, 3] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 7th Trimino
    matrix[2, 5] = currentLetter;
    matrix[2, 6] = currentLetter;
    matrix[3, 5] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 8th Trimino
    matrix[3, 1] = currentLetter;
    matrix[4, 0] = currentLetter;
    matrix[4, 1] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 9th Trimino
    matrix[3, 4] = currentLetter;
    matrix[4, 3] = currentLetter;
    matrix[4, 4] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 10th Trimino
    matrix[3, 6] = currentLetter;
    matrix[4, 5] = currentLetter;
    matrix[4, 6] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 11th Trimino
    matrix[4, 2] = currentLetter;
    matrix[5, 1] = currentLetter;
    matrix[5, 2] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 12th Trimino
    matrix[5, 0] = currentLetter;
    matrix[6, 0] = currentLetter;
    matrix[6, 1] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 13th Trimino
    matrix[5, 3] = currentLetter;
    matrix[6, 2] = currentLetter;
    matrix[6, 3] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 14th Trimino
    matrix[5, 4] = currentLetter;
    matrix[5, 5] = currentLetter;
    matrix[6, 4] = currentLetter;

    currentLetter = NextLetter(currentLetter, matrix, (0, 0));

    // 15th Trimino
    matrix[5, 6] = currentLetter;
    matrix[6, 5] = currentLetter;
    matrix[6, 6] = currentLetter;

    return currentLetter;
}

static char Generate2x2Block(int row, int col, char letter, char[,] matrix)
{
    char currentLetter = NextLetter(letter, matrix, (row, col), (row, col + 1), (row + 1, col), (row + 1, col + 1));

    matrix[row, col] = currentLetter;
    matrix[row, col + 1] = currentLetter;
    matrix[row + 1, col] = currentLetter;
    matrix[row + 1, col + 1] = currentLetter;

    return currentLetter;
}

static char GenerateHorizontalSlice(int size, char letter, char[,] matrix)
{
    int colIndex = 0;
    char currentLetter = NextLetter(letter, matrix, (size, colIndex), (size + 1, colIndex), (size, colIndex + 1));

    // Starting trimino
    matrix[size, colIndex] = currentLetter;
    matrix[size + 1, colIndex] = currentLetter;
    colIndex++;
    matrix[size, colIndex] = currentLetter;

    // S tetrominos
    int numberOfTetrominos = (size * 2 - 6) / 4;
    for (int i = 0; i < numberOfTetrominos; i++)
    {
        currentLetter = NextLetter(currentLetter, matrix, (size, colIndex + 1), (size + 1, colIndex), (size + 1, colIndex + 1), (size, colIndex + 2));
        matrix[size + 1, colIndex] = currentLetter;
        colIndex++;
        matrix[size, colIndex] = currentLetter;
        matrix[size + 1, colIndex] = currentLetter;
        colIndex++;
        matrix[size, colIndex] = currentLetter;
    }

    // Ending trimino
    currentLetter = NextLetter(currentLetter, matrix, (size, colIndex + 1), (size + 1, colIndex), (size + 1, colIndex + 1));
    matrix[size + 1, colIndex] = currentLetter;
    colIndex++;
    matrix[size, colIndex] = currentLetter;
    matrix[size + 1, colIndex] = currentLetter;

    return currentLetter;
}

static char GenerateVerticalSlice(int size, char letter, char[,] matrix)
{
    int rowIndex = 0;
    char currentLetter = NextLetter(letter, matrix, (rowIndex, size), (rowIndex, size + 1), (rowIndex + 1, size));

    // Starting trimino
    matrix[rowIndex, size] = currentLetter;
    matrix[rowIndex, size + 1] = currentLetter;
    rowIndex++;
    matrix[rowIndex, size] = currentLetter;

    // Z tetrominos
    int numberOfTetrominos = (size * 2 - 6) / 4;
    for (int i = 0; i < numberOfTetrominos; i++)
    {
        currentLetter = NextLetter(currentLetter, matrix, (rowIndex, size + 1), (rowIndex + 1, size + 1), (rowIndex + 1, size), (rowIndex + 2, size));
        matrix[rowIndex, size + 1] = currentLetter;
        rowIndex++;
        matrix[rowIndex, size] = currentLetter;
        matrix[rowIndex, size + 1] = currentLetter;
        rowIndex++;
        matrix[rowIndex, size] = currentLetter;
    }

    // Ending trimino
    currentLetter = NextLetter(currentLetter, matrix, (rowIndex + 1, size), (rowIndex, size + 1), (rowIndex + 1, size + 1));
    matrix[rowIndex, size + 1] = currentLetter;
    rowIndex++;
    matrix[rowIndex, size] = currentLetter;
    matrix[rowIndex, size + 1] = currentLetter;

    return currentLetter;
}

static char NextLetter(char currentLetter, char[,] matrix, params (int, int)[] points)
{
    char next = (char)(currentLetter + 1);
    if (next > 'z')
    {
        next = 'a';
    }

    while (true)
    {
        bool conflict = false;

        int[] dr = [-1, 1, 0, 0];
        int[] dc = [0, 0, -1, 1];

        foreach ((int r, int c) in points)
        {
            for (int i = 0; i < 4; i++)
            {
                int nr = r + dr[i];
                int nc = c + dc[i];

                if (nr >= 0 && nr < matrix.GetLength(0) && nc >= 0 && nc < matrix.GetLength(1))
                {
                    if (matrix[nr, nc] == next)
                    {
                        conflict = true;
                        break;
                    }
                }
            }
        }

        if (!conflict)
        {
            break;
        }

        next = (char)(next + 1);
        if (next > 'z')
        {
            next = 'a';
        }
    }

    return next;
}