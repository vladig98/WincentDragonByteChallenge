namespace BinaryWithATwist
{
    public static class BaseConverter
    {
        public static long FindBase(long number)
        {
            if (number == 0 || number == 1 || number == 3)
            {
                return 3;
            }

            if (number < 0 || number == 2)
            {
                return -1;
            }

            long upperBound = (long)Math.Sqrt(number);

            for (int currentBase = 3; currentBase <= upperBound; currentBase++)
            {
                long numberCopy = number;
                bool exitedEarlier = false;

                while (numberCopy > 0)
                {
                    long reminder = numberCopy % currentBase;

                    if (reminder != 0 && reminder != 1)
                    {
                        exitedEarlier = true;
                        break;
                    }

                    numberCopy /= currentBase;
                }

                if (exitedEarlier)
                {
                    continue;
                }

                return currentBase;
            }

            return number - 1;
        }
    }
}
