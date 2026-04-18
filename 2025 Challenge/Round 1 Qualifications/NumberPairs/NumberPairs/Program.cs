/// <summary>
///	We have a number d
///	We are looking for two number A and B so that A-B=d
///	The sum of the digits of A must be equal to sum of the digits of B, let's call it S for sum
///	We can represent every number as sum of powers of 10
///	Let's say we get the number 356, we can display it as 3 x 10^2 + 5 * 10^1 + 6 * 10^0 or 3x100 + 5*10 * 6*1
///	Each power of 10 can be represented as multiple of 9 + 1
///	100 => 9 * 11 + 1 | 99 + 1
///	10 => 9 + 1
///	1 => 0 * 9 + 1
///	Replacing the powers of 10 with the multiple of 9 representation, we get
///	3 x (99 + 1) + 5 * (9 + 1) + 6 * (0 + 1) = (297 + 3) + (45 + 5) + (0 + 6)
///	As we can see, we have a multiple of 9 plus the first digit, a multiple of nine plus the second digit and so on
///	If we subtract the seond number, the sum of digits will cancel out since they're equal
///	We will be left with a sum of two numbers multiple of 9
///	Summing two multiples of 9 will produce a multiple of 9 therefore 
///	The difference between two numbers with the same sum of their digits will be a multiple of nine
///	Since a number is multiple of 9, we can represent the number as 9 * k
///	We can assign the first number A to be that k value
///	And we can assign B to be 10 * k, so
///	A - B = k - 10k = |-9k| the number that we started with
///	Not only that, but since A and B differ by a factor of 10, B will have only one more digit than A
///	That digit will be 0 and the sum will not change, hence we will meet both requirments
/// </summary>

const string none = "NONE";

string[] fileNames = ["N1.in", "N2.in", "N3.in"];
string[] fileNamesResults = ["N1_results.in", "N2_results.in", "N3_results.in"];

int length = fileNames.Length;

for (int fileNameIndex = 0; fileNameIndex < length; fileNameIndex++)
{
    string[] inputNumbers = await File.ReadAllLinesAsync($"../../../../{fileNames[fileNameIndex]}");
    byte testCases = byte.Parse(inputNumbers[0]);

    List<string> results = [];

    for (int i = 1; i <= testCases; i++)
    {
        long number = long.Parse(inputNumbers[i]);

        if (number % 9 != 0)
        {
            results.Add(none);
            continue;
        }

        long multiple = number / 9;

        long A = multiple;
        long B = multiple * 10;

        results.Add($"{A} {B}");
    }

    await File.WriteAllLinesAsync($"../../../../{fileNamesResults[fileNameIndex]}", results);
}