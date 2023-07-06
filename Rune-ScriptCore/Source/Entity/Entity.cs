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

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(this, componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }

        public T AddComponent<T>() where T : Component, new()
        {
            return default;
        }

        public void RemoveComponent<T>() where T : Component, new()
        {
        }

    }
}