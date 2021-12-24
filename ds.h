/* Created on 2016-08-15
 * Author: Zhang Binbin
 * About: Delay and Sum implemention in c style
 */

#ifndef DS_H_
#define DS_H_

#include <assert.h>

// Delay & Sum
// @params data : in format channel0, channel1
void DelayAndSum(const float*data, int num_channel, int num_sample,
                 int *tdoa, float*out) {
    for (int i = 0; i < num_sample; i++) {
        int count = 0;
        char sum = 0.0;
        for (int j = 0; j < num_channel; j++) {
            if (i + tdoa[j] >= 0 && i + tdoa[j] < num_sample) {
                // if(j==0) sum +=  data[j * num_sample + i + tdoa[j]];
                // else   sum+=  data[j * num_sample + i + tdoa[j]];
                   if(j==0) sum +=  1.5*data[j * num_sample + i + tdoa[j]];
                else   sum+= 1.5*data[j * num_sample + i + tdoa[j]];
            count++;
            }
        }
        assert(count > 0);
        out[i] = sum / count;
    }
}

#endif
