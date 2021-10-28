#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
	#include <OpenCL/opencl.h>
#else
	#include <CL/cl.h>
#endif

#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)

int main() {
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
	int v1[4] = { 4, 3, 2, 1 };
	int v2[4] = { 4, 3, 2, 1 };
	int vOut[4];

	FILE* fp;
	char fileName[] = "./kernal.cl";
	char* source_str;
	size_t source_size;

	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}

	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	/* Get Platform and Device Info */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* Create OpenCL context */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Create Memory Buffer */
	cl_mem v1gpu = clCreateBuffer(context, CL_MEM_READ_WRITE, 4 * sizeof(int), NULL, &ret);
	cl_mem v2gpu = clCreateBuffer(context, CL_MEM_READ_WRITE, 4 * sizeof(int), NULL, &ret);
	cl_mem v2gpuOut = clCreateBuffer(context, CL_MEM_READ_WRITE, 4 * sizeof(int), NULL, &ret);

	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(context, 1, (const char**)&source_str,
		(const size_t*)&source_size, &ret);

	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "matrix_multiply", &ret);

	/* Set OpenCL Kernel Parameters */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&v1gpu);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&v2gpu);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&v2gpuOut);

	/* Copy data host to gpu */
	ret = clEnqueueWriteBuffer(command_queue, v1gpu, CL_TRUE, 0, 4 * sizeof(int), v1, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, v2gpu, CL_TRUE, 0, 4 * sizeof(int), v2, 0, NULL, NULL);
	/* Execute OpenCL Kernel */
	//ret = clEnqueueTask(command_queue, kernel, 0, NULL, NULL);

	size_t globalSize[1] = { 4 };
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, globalSize, NULL, 0,
		NULL, NULL);

	/* Copy results from the memory buffer */
	ret = clEnqueueReadBuffer(command_queue, v2gpuOut, CL_TRUE, 0, 4 * sizeof(int), vOut, 0, NULL, NULL);
	/* Display Result */
	for (int i = 0; i < 4; i++)
		printf("%d * %d = %d \n", v1[i], v2[i], vOut[i]);

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
	free(source_str);

	return 0;
}