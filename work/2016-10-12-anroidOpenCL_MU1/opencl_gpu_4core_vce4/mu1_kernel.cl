


__kernel void process_iq(__global void *in_i, __global void *in_q, __global void *output) {


    int i = get_global_id(0)*64;
    int j = get_global_id(1);

    __global int16 *table_i = in_i;
    __global int16 *table_q = in_q;
    __global int16 *table_o = output;


    table_o[i+j] = (table_i[i+j] + table_q[i+j]) / (table_i[i+j] - table_q[i+i])  + (table_i[i+j] + table_q[i+j] - (int16)1) / (table_i[i+j] - table_q[i+j] + (int16)1);


}


