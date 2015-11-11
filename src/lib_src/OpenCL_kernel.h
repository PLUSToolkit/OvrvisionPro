const char kernel[] = 
"__constant sampler_t LINEAR = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;\n"
"__constant sampler_t NEAREST = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;\n"
"\n"
"__kernel void remap(\n"
"__read_only image2d_t src,		// CL_UNSIGNED_INT8 x 4\n"
"__read_only image2d_t mapX,		// CL_FLOAT\n"
"__read_only image2d_t mapY,		// CL_FLOAT\n"
"__write_only image2d_t	dst)	// CL_UNSIGNED_INT8 x 4\n"
"{\n"
"	int x = get_global_id(0);\n"
"	int y = get_global_id(1);\n"
"	float X = read_imagef(mapX, (int2)(x, y)).x;\n"
"	float Y = read_imagef(mapY, (int2)(x, y)).x;\n"
"	uint4 pixel = read_imageui(src, LINEAR, (float2)(X, Y));\n"
"	write_imageui(dst, (int2)(x, y), pixel);\n"
"}\n"
"\n"
"__kernel void demosaic(\n"
"		__read_only image2d_t src,	// CL_UNSIGNED_INT16\n"
"		__write_only image2d_t left,	// CL_UNSIGNED_INT8 x 4\n"
"		__write_only image2d_t right)	// CL_UNSIGNED_INT8 x 4\n"
"{\n"
"}\n"
;