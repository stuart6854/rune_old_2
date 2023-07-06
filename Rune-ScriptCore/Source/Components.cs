namespace Rune
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class Transform : Component
    {
        public Vector3 Position
        {
            get
            {
                InternalCalls.Entity_GetPosition(Entity, out Vector3 Position);
                return Position;
            }
            set
            {
                InternalCalls.Entity_SetPosition(Entity, ref value);
            }
        }

        public Vector3 EulerAngles
        {
            get
            {
                InternalCalls.Entity_GetPosition(Entity, out Vector3 Position);
                return Position;
            }
            set
            {
                InternalCalls.Entity_SetPosition(Entity, ref value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.Entity_GetPosition(Entity, out Vector3 Position);
                return Position;
            }
            set
            {
                InternalCalls.Entity_SetPosition(Entity, ref value);
            }
        }
    }
}
