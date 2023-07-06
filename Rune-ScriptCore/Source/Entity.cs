using System;

namespace Rune
{
    public class Entity
    {
        private uint _id;

        public static Entity Create()
        {
            Console.WriteLine("Entity::Create()");
            return new Entity(0);
        }

        private Entity(uint id)
        {
            _id = id;
        }

        public void Destroy()
        {
            Console.WriteLine("Entity::Destroy()");
        }

        public T GetComponent<T>()
        {
            Console.WriteLine("Entity::GetComponent<T>()");
            return default;
        }

        public T AddComponent<T>()
        {
            Console.WriteLine("Entity::AddComponent<T>()");
            return default;
        }

        public void RemoveComponent<T>()
        {
            Console.WriteLine("Entity::RemoveComponent<T>()");
        }
    }
}