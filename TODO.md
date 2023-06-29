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

- [x] Scene/World
  - [ ] Components - Transform, StaticRenderer, Camera, Light
  - [ ] Loading from disk
- [ ] Resource Manager
- [ ] Add camera controls
- [ ] Use Synty assets to built test scene
