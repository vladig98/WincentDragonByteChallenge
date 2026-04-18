namespace NumberPairs
{
    public static class NumberFinder
    {
        public static (ulong, ulong) Find(ulong number)
        {
            ulong real = number;
            long sumDifference = 0;
            long carryOverA = 0;

            int numberLength = number.ToString().Length;
            List<long> numberADigits = [];
            List<long> numberBDigits = [];

            for (int j = 0; j < numberLength; j++)
            {
                long digit = (long)number % 10;

                long canditateB = digit + 1;
                long canditateA = 0;

                if (j > 0 && sumDifference == 0)
                {
                    canditateB = digit;
                }
                else if (sumDifference > 0)
                {
                    canditateB = sumDifference + digit;
                }
                else if (sumDifference < 0)
                {
                    canditateB = -sumDifference;
                }

                if (canditateB >= 10)
                {
                    canditateB = 9;
                }

                canditateA = digit + canditateB + carryOverA;
                carryOverA = 0;

                if (canditateA >= 10 && sumDifference < 0)
                {
                    carryOverA += canditateA / 10;
                    canditateA = 9;
                    canditateB = canditateA - digit - carryOverA;

                    carryOverA = 0;
                }
                else if (canditateA >= 10)
                {
                    carryOverA += canditateA / 10;
                    canditateA %= 10;
                }

                sumDifference += canditateA - canditateB;

                numberADigits.Add(canditateA);
                numberBDigits.Add(canditateB);

                number /= 10;
            }

            numberADigits.Add(carryOverA);

            numberADigits.Reverse();
            numberBDigits.Reverse();

            ulong numberA = ulong.Parse(string.Join("", numberADigits));
            ulong numberB = ulong.Parse(string.Join("", numberBDigits));

            return (numberA, numberB);
        }
    }
}
