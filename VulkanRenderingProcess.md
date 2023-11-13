

- we need a framebuffer to render anything
- a framebuffer contains image views which will be used for each of the
    attachments (colors, depth, stencil, etc.)
- the framebuffer will be used with the render pass
- anytime work is done, we record commands.
- example of work are: drawing, presenting, transferring memory
- commands are recorded in a command buffer, which is then submitted in queue
- vulkan will re order the commands in an optimal way
- before we use command buffers, we need a command pool
- command pool allocates command buffers
- the idea of a pool is that it holds a bunch of threads and allows
    them to spin in idle (reduces overhead)
- command bool manages memory of command buffers (no need to destroy buffers individually)

= how to set up an engine to use commands:

- set up a program that has a bunch of frames
- each frame has its own swapchain, command buffer
- when the program wants to draw to frame 1, the commands are
    recorded in that frames command buffer
- we will also have a main worker command buffer

if we want to draw something
1. fetch frame
2. record commands on that frames command buffer
3. submit command buffer (actually performs the drawing commands)
4. present the frame

frames can be idle. when needed, we will acquire an idle frame, and write to it.
it then becomes a rendered frame. we will then pick a rendered frame
and present it

mailbox mode. if the idle frame section is all empty, all frames are either
rendered or presenting, then the acquire operation can take from already rendered frames

drawfunction.  acquire image, graphics operations, and presentation.
a function does two things. it does its operations and returns, and the GPU works (slow)
sometimes a function can return before the image is complete (esp. during graphics operations)
so we use a semaphore/fence operation to reserve resources. 
The acquire process will signal when the GPU has acquired a valid image.
The graphics operation will signal the present operation when the gpu
has finished drawing.