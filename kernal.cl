__kernel void matrix_multiply(__global int* v1, __global int* v2, __global int* vectorOut) {
	const int index = get_global_id(0);
	vectorOut[index] = v1[index] * v2[index];
}