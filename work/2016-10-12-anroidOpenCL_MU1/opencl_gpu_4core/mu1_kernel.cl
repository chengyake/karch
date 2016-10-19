


__kernel void process_iq(__global void *in_i, __global void *in_q, __global void *output) {



    int i = get_global_id(0)*1024;
    int j = get_global_id(1);
    __global int *table_i = in_i;
    __global int *table_q = in_q;
    __global int *table_o = output;
    
    table_o[i+j] = (table_i[i+j] + table_q[i+j]) / (table_i[i+j] - table_q[i+j])  + (table_i[i+j] + table_q[i+j] - 1) / (table_i[i+j] - table_q[i+j] + 1);

    //table_o[i+j] += (table_i[i+j] + table_q[i+j] - 1) / (table_i[i+j] - table_q[i+j] + 1);

}


