namespace Rune
{
    public static class Time
    {
        public static float DeltaTime => InternalCalls.Time_GetDelta();
        public static float SinceStartup => InternalCalls.Time_GetSinceStartup();
    }
}
