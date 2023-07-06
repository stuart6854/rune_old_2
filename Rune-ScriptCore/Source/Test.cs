using System;

namespace Rune
{
    class Test
    {
        public Test()
        {
            Console.WriteLine("Main Constructor");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World From C#");
        }

        public void PrintInt(int value)
        {
            Console.WriteLine($"C# says: {value}");
        }

        public void PrintString(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }
    }
}