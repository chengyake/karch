__kernel void mmap(__global char* in, __global char* out)
{
	int num = get_global_id(0);
	in[num] = in[num] + 1;
}

__kernel void mmap1(__global char* in, __global char* out)
{
	int num = get_global_id(0);
	in[num] = in[num] + 1;
}

