

vertex input => input assembly => vertex shader => tesselation => geometry shader =>
	rasterization => fragment shader => color blend ==> appropriate attachments

= vertex input 
- binding descriptions: info about the buffer itself given to pipeline
    - binding #, stride, input rate, etc.
- attribute description: location, binding, format, offset (think attr ptr)
- 

= input assembler
- biggest concern is the topology type:
- lines mode: take first two points, stick them together, send them through
- points mode: take each point and pass it through
- triangle mode: take first 3 points, pass through
- triangle strip: take first 3 points, and then next two, and make two triangles
- index buffer handled differently

= vertex shader
- for a given primitive, the vertex shader runs once on each of its vertices
- works out any per-vertex data (position, color, alpha) for each vertex

= tesselation shader
- can subdivide geometry arbitrarily (mesh subidivisions) add LOD based on distance
- (if noop, then its the identity operation)

= goemetry shader
- takes the output of the tess shader 
- triangle adjacency mode: has reference to the nearby vertices of neighboring triangles

(mid-step)
- viewport and scissor test cut the region up and display only what overlaps
    between the two regions

= rasterization shader
- takes this per-vertex data, and interpolate it using barycentric coordinates
- depth testing is performed at this state
- backface culling is performed at this stage

= fragment shader
- for each interpolated barycentric coordinate (read: fragment) it calculates the pixel color
- after frag shader we can multi sample


= color blend
- has two fragments, dst and src, and blend them using some algorithm
- (e.g if we have a 10% transparency color, and we want to pass it to a 90% pixel of a different color)

Vulkan has a struct called a shader stage:
- specify stage
- specify module (compiled shader code spirv)
- entry point (name of the main function)

= pipeline layout: describing what data the pipeline can take as it runs
- bindings: hooks to where we can pass values in
- push constants (work sort of similar to uniforms for small data)
- descriptor sets: like uniforms for large data

The whole pipeline can be defined as a "render pass"

= render pass
- what attachments are we working with? (color, depth, resolve, etc.)
- each attachment has a: 
    - load operation: load data, clear data
    - store operation: store data
    - stencil load: 
    - stencil store, 
    - initial layout, 
    - final layout

= sub pass: always need a subpass 
- like running the pipeline again but with data from the previous render pass
- useful in methods like deferred shading (rendering everything visible on screen)
- we can take all that data and pass it through the subpass where light is calculated
- subpass has attachments
    - color
    - depth stencil
    - resolve: resolves multiple colors when using multisampling
    - preserve