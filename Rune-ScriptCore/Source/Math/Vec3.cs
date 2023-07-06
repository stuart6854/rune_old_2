using System;

namespace Rune
{
    public struct Vector3
    {
        public float x { get; set; }
        public float y { get; set; }
        public float z { get; set; }

        public Vector3(float x = 0, float y = 0, float z = 0)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public float Length() => (float)Math.Sqrt((double)(x * x + y * y + z * z));

        public override string ToString()
        {
            return $"{x}, {y}, {z}";
        }
    }
}
