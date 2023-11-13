

suppose we have vertex attributes:
{pos, color, tex, normal, tangent, ..}

human readable:
	pos = [p1_x, p1_y, p2_x, p2_y, ...]
	color = [p1_r, p1_g, p1_b, ...]

flat:
	vertices = {p1_x, p1_y, p1_r, ...., p2_x, p2_y, ...}

A vertex array object (VAO) holds a vertex buffer object (holds size and data)
and attribute pointers. for each attribute, we have an attribute number, offset, 
stride, etc.

in vulkan, we have a binding description, with a binding number (id), stride (number of bytes
per vertex), and input rate (per instance data, per instance data, etc.)).  
similar to GPU bind groups. In addition we have attribute descriptions.

Each attr. desc. with be associated with a binding number (the same # as the binding
description) but will have their own 'location'. We have a format (what data type
and how many elements) and the data offset where it begins

when we are creating a pipeline, we specify the binding description and attr desc. at
pipeline creation (at the vertex input stage) the "VertexInputInfo"