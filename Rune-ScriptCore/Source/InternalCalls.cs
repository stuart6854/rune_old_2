using System.Runtime.CompilerServices;

namespace Rune
{
    internal static class InternalCalls
    {
        #region Time

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float Time_GetDelta();

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float Time_GetSinceStartup();

        #endregion

        #region Entity

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Entity Entity_Create(out Entity entity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_Destroy(ref Entity entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_AddPosition(Entity entity, ref Vector3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_GetComponent(Entity entity, out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_GetPosition(Entity entity, out Vector3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetPosition(Entity entity, ref Vector3 position);

        #endregion

    }
}
