using System;
using Rune;

namespace Sandbox
{
    public class Camera
    {
        public static void Update(Entity entity)
        {
            Vector3 position = entity.GetComponent<Transform>().Position;
            position.y = (float)Math.Sin((double)Time.SinceStartup) * 1.0f + 1.0f;
            entity.GetComponent<Transform>().Position = position;
        }
    }
}
