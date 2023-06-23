# Design Notes

## Builds

- Debug - Unoptimized + Debug code (assets, console logging, etc.)
- Release - Optimized + Debug code
- Distro - Optimized + No debug code

## Error Handling

1) Is error recoverable? 
   1) Log error and continue...
2) Error is unrecoverable.
   1) Log error & Throw exception (stack will rewind until it hits a try/catch block).

Engine entrypoint should have a try/catch block which will, when an exception is caught, flush the logging system then close the program. 
