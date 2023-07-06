using System;
using Rune;

namespace Sandbox
{
    public static class Example
    {
        public static void Update(Entity entity)
        {
            //Console.WriteLine("Sandbox.Example::Update(Entity)");

            Vector3 position = entity.GetComponent<Transform>().Position;
            position.y = (float)Math.Sin((double)Time.SinceStartup) * 1.0f + 1.0f;
            entity.GetComponent<Transform>().Position = position;

            //Console.WriteLine($"{position}");
        }
    }
}
