using System;

namespace Rune
{
    public class Entity
    {
        private readonly uint _id;

        public static Entity Create()
        {
            InternalCalls.Entity_Create(out Entity entity);
            return entity;
        }
        public static void Destroy(ref Entity entity)
        {
            InternalCalls.Entity_Destroy(ref entity);
            entity = null;
        }

        private Entity(uint id)
        {
            _id = id;
        }

        public T GetComponent<T>() where T : Component, new()
        {
            Console.WriteLine("Entity::GetComponent<T>()");
            return default;
        }

        public T AddComponent<T>() where T : Component, new()
        {
            Console.WriteLine("Entity::AddComponent<T>()");
            return default;
        }

        public void RemoveComponent<T>() where T : Component, new()
        {
            Console.WriteLine("Entity::RemoveComponent<T>()");
        }

    }
}