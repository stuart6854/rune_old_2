# TODO

- Low-Level
  - Messaging system
    - Id - StringId = SID("SomeMessageId") == 723647823647
    - Payload - https://khuttun.github.io/2015/06/01/polymorphic-message-queue-in-c++11.html
    - Support immediate & queued messages
    - Listeners register themselves
  - IO layer
  - Graphics layer - Vulkan wrapper
  - Network layer??
- High-Level
  - Scene/World system
  - Scene rendering
  - Resource manager
  - Scripting - C#
- Misc
  - Utility
    - Color type
    - Random

# Current Plan

1) Scene/World
  1) Components - Transform, StaticRenderer, Camera, Light
  2) Loading from disk
2) Resource Manager
3) Add camera controls
4) Use Synty assets to built test scene
