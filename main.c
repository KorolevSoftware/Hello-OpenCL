#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
	#include <OpenCL/opencl.h>
#else
	#include <CL/cl.h>
#endif

#include <kernal_cl.h>
#include <string.h>

#define array_count(x) sizeof(x)/sizeof(x[0])


int main() {
	cl_device_id device_id;
	cl_context context;
	cl_command_queue command_queue;
	cl_program program;
	cl_kernel kernel;
	cl_platform_id platform_id;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;

	int input_vector_1[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
	int input_vector_2[] = { 1, 2, 3, 4,5, 6, 7, 8, 9 };
	int output_vector[array_count(input_vector_2)];

	/* Get Platform and Device Info */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* Create OpenCL context */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Create Memory Buffer */
	cl_mem v1gpu = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(input_vector_1), NULL, &ret);
	cl_mem v2gpu = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(input_vector_2), NULL, &ret);
	cl_mem v2gpuOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(output_vector), NULL, &ret);

	/* Create Kernel Program from the source */
	const char** kernal_soure_code[] = { kernal_cl_data };

	program = clCreateProgramWithSource(context, array_count(kernal_soure_code), kernal_soure_code,
		NULL, &ret);

	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "matrix_multiply", &ret);

	/* Set OpenCL Kernel Parameters */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&v1gpu);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&v2gpu);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&v2gpuOut);

	/* Copy data host to gpu async */
	cl_event write_events[2];

	ret = clEnqueueWriteBuffer(command_queue, v1gpu, CL_TRUE, 0, sizeof(input_vector_1), input_vector_1, 0, NULL, &write_events[0]);
	ret = clEnqueueWriteBuffer(command_queue, v2gpu, CL_TRUE, 0, sizeof(input_vector_2), input_vector_2, 0, NULL, &write_events[1]);
	/* Execute OpenCL Kernel */

	clWaitForEvents(2, write_events); // weit data copy to gpu 

	size_t globalSize[1] = { array_count(output_vector) };
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, globalSize, NULL, 0,
		NULL, NULL);

	/* Copy results from the memory buffer */
	ret = clEnqueueReadBuffer(command_queue, v2gpuOut, CL_TRUE, 0, sizeof(output_vector), output_vector, 0, NULL, NULL);
	/* Display Result */
	for (int i = 0; i < array_count(output_vector); i++)
		printf("%d * %d = %d \n", input_vector_1[i], input_vector_2[i], output_vector[i]);

	/* Finalization */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(v1gpu);
	ret = clReleaseMemObject(v2gpu);
	ret = clReleaseMemObject(v2gpuOut);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	getchar();
	return 0;
}