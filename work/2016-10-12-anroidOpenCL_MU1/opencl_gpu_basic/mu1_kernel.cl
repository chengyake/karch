


__kernel void process_iq(__global void *in_i, __global void *in_q, __global void *output) {



    int i = get_global_id(0);
    int j = get_global_id(1);
    __global int *table_i = in_i;
    __global int *table_q = in_q;
    __global int *table_o = output;

    table_o[i*1024+j] = (table_i[i*1024+j] + table_q[i*1024+j]) / (table_i[i*1024+j] - table_q[i*1024+j]);


}


