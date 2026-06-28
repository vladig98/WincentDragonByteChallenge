using System.Numerics;

string[] inputFiles = ["W0_example.in", "W1.in", "W2.in", "W3.in", "W4.in"];
string[] outputFiles = ["W0_example.out", "W1.out", "W2.out", "W3.out", "W4.out"];

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

        int testcases = int.Parse(headerLine);
        for (int t = 0; t < testcases; t++)
        {
            string? caseData = reader.ReadLine();
            if (caseData == null)
            {
                break;
            }

            string[] inputTokens = caseData.Split(' ');
            int numberOfPoints = int.Parse(inputTokens[0]);
            int numberOfSlides = int.Parse(inputTokens[1]);

            caseData = reader.ReadLine();
            if (caseData == null)
            {
                break;
            }

            int[] heights = [.. caseData.Split(' ').Select(int.Parse)];

            Station[] stations = new Station[numberOfPoints];
            for (int idx = 0; idx < numberOfPoints; idx++)
            {
                stations[idx] = new Station(idx, heights[idx]);
            }

            Array.Sort(stations, (a, b) => b.Height.CompareTo(a.Height));

            object scoreLock = new();
            BigInteger globalBestScore = -1;
            SlideOptions? globalBestOption = null;

            Parallel.For(0, stations.Length - 1, new ParallelOptions() { MaxDegreeOfParallelism = Environment.ProcessorCount }, startI =>
            {
                List<Slide> localSlides = [];
                Stack<int> localOuterStates = [];
                Stack<int> localInnerStates = [];
                bool[] localUsedVertices = new bool[numberOfPoints];
                BigInteger localCurrentScore = 0;

                int i = startI;
                int j = i + 1;

                bool isRootBranch = true;

                do
                {
                    if (i >= stations.Length - 1)
                    {
                        goto triggerBacktrack;
                    }

                    var startNode = stations[i];
                    var endNode = stations[j];

                    BigInteger currentBestBenchmark;
                    lock (scoreLock)
                    {
                        currentBestBenchmark = globalBestScore;
                    }

                    // Branch and Bound Pruning
                    if (currentBestBenchmark != -1)
                    {
                        int slidesNeeded = numberOfSlides - localSlides.Count;
                        BigInteger maxPossibleScore = localCurrentScore;
                        int slidesCounted = 0;

                        for (int k = i; k < stations.Length && slidesCounted < slidesNeeded; k++)
                        {
                            maxPossibleScore += (stations[k].Height - stations[^1].Height);
                            slidesCounted++;
                        }

                        if (slidesCounted < slidesNeeded || maxPossibleScore <= currentBestBenchmark)
                        {
                            goto forceBacktrack;
                        }
                    }

                    // Inclusion Phase
                    if (!localUsedVertices[startNode.Index] && !localUsedVertices[endNode.Index])
                    {
                        if (!localSlides.Any(x => Crosses(x.Start, x.End, startNode.Index, endNode.Index)))
                        {
                            localOuterStates.Push(i);
                            localInnerStates.Push(j);

                            localSlides.Add(new Slide(startNode.Index, endNode.Index, startNode.Height, endNode.Height));
                            localCurrentScore += (startNode.Height - endNode.Height);

                            localUsedVertices[startNode.Index] = true;
                            localUsedVertices[endNode.Index] = true;

                            if (localSlides.Count == numberOfSlides)
                            {
                                lock (scoreLock)
                                {
                                    if (globalBestOption == null || localCurrentScore > globalBestScore)
                                    {
                                        globalBestScore = localCurrentScore;
                                        globalBestOption = new SlideOptions([.. localSlides]);
                                    }
                                }

                                goto forceBacktrack;
                            }

                            i++;
                            j = i + 1;
                            isRootBranch = false;

                            continue;
                        }
                    }

                    j++;
                    if (j >= stations.Length)
                    {
                        i++;
                        j = i + 1;
                    }
                    continue;

                forceBacktrack:
                    i = stations.Length;

                triggerBacktrack:
                    if (i >= stations.Length - 1)
                    {
                        bool wentBack = localOuterStates.TryPop(out int oldI);
                        localInnerStates.TryPop(out int oldJ);

                        if (!wentBack || (localSlides.Count == 0 && isRootBranch))
                        {
                            break;
                        }

                        if (localSlides.Count > 0)
                        {
                            Slide removed = localSlides[^1];
                            localCurrentScore -= removed.Difference;

                            localUsedVertices[removed.Start] = false;
                            localUsedVertices[removed.End] = false;

                            localSlides.RemoveAt(localSlides.Count - 1);
                        }

                        i = oldI;
                        j = oldJ + 1;

                        if (j >= stations.Length)
                        {
                            i++;
                            j = i + 1;
                        }
                    }
                } while (true);
            });

            var bestOption = globalBestOption;

            if (!bestOption.HasValue)
            {
                writer.WriteLine("-1 -1");
            }
            else
            {
                foreach (Slide slide in bestOption.Value.Slides)
                {
                    writer.WriteLine($"{slide.Start} {slide.End}");
                }
            }
        }
    }

    Console.WriteLine($"Finished: {outputFiles[f]}\n");
}

static bool Crosses(int a, int b, int c, int d)
{
    if (a == c || a == d || b == c || b == d)
    {
        return true;
    }

    return Between(a, c, b) != Between(a, d, b);
}

static bool Between(int start, int point, int end)
{
    if (start < end)
    {
        return start < point && point < end;
    }

    return point > start || point < end;
}

readonly record struct Station(int Index, int Height);

readonly record struct Slide(int Start, int End, int StartHeight, int EndHeight)
{
    public int Difference => StartHeight - EndHeight;
}

readonly record struct SlideOptions(List<Slide> Slides);