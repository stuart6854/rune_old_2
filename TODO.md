# TODO

- Low-Level
  - Messaging system
    - Id - StringId = SID("SomeMessageId") == 723647823647
    - Payload - https://khuttun.github.io/2015/06/01/polymorphic-message-queue-in-c++11.html
    - Support immediate & queued messages
    - Listeners register themselves
  - IO layer
  - [x] Graphics layer - Vulkan wrapper
  - Network layer??
- High-Level
  - Input system
    - Action binding (keys)
    - Axis binding (mouse, joystick)
  - [x] Scene/World system
  - [x] Scene rendering
  - [x] Resource manager
  - Scripting - C#
- Misc
  - Utility
    - Color type
    - Random

# Current Plan

- [x] Scene/World
  - [ ] Components - Transform, StaticRenderer, Camera, Light
  - [x] Loading from disk
- [x] Resource Manager
- [x] Add camera controls
- [ ] Use Synty assets to built test scene
