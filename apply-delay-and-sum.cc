/* Created on 2016-08-16
 * Author: Zhang Binbin
 */

#include <stdio.h>

#include "wav.h"
#include "tdoa.h"
#include "ds.h"
#include "parse-option.h"
#include<iostream>
using namespace std;

//to calculate the covariance matrix
int covariance(const float*data, float* out_pcm,int num_channel,int numsample){
    double cov [num_channel+1];
    double mean[7]={0};
    for (int i = 0; i < numsample; i++) {
        mean[0]+=out_pcm[i];
        mean[1]+=data[i*num_channel+0];
        mean[2]+=data[i*num_channel+1];
        mean[3]+=data[i*num_channel+2];
        mean[4]+=data[i*num_channel+3];
        mean[5]+=data[i*num_channel+4];
        mean[6]+=data[i*num_channel+5];
    }
        mean[0]/=numsample;
        mean[1]/=numsample;
        mean[2]/=numsample;
        mean[3]/=numsample;
        mean[4]/=numsample;
        mean[5]/=numsample;
        mean[6]/=numsample;
    
         for(int j=0;j<numsample;j++){
             cov[1]+=(out_pcm[j]-mean[0])*(data[j*num_channel+0]-mean[1]);
             cov[2]+=(out_pcm[j]-mean[0])*(data[j*num_channel+1]-mean[2]);
             cov[3]+=(out_pcm[j]-mean[0])*(data[j*num_channel+2]-mean[3]);
             cov[4]+=(out_pcm[j]-mean[0])*(data[j*num_channel+3]-mean[4]);
             cov[5]+=(out_pcm[j]-mean[0])*(data[j*num_channel+4]-mean[5]);
             cov[6]+=(out_pcm[j]-mean[0])*(data[j*num_channel+5]-mean[6]);
         }
         double max=0;
         int chnl=0;
         for ( int i = 1; i <= num_channel; i++)
         {
              if(max<cov[i]) {
                  max=cov[i];
                  chnl=i;
              }
         }
         return max, chnl;
}

int main(int argc, char *argv[]) {

    const char *usage = "Do delay and sum beamforming\n"
                        "Usage: apply-delay-and-sum multi_channel_file output_file\n";
    ParseOptions po(usage);

    int tdoa_window = 4000;
    po.Register("tdoa-window", &tdoa_window, 
                "window size for estimated tdoa, in sample point");
    int beam_window = 4000;
    po.Register("beam-window", &beam_window, 
                "window size for delay and sum, less than tdoa-window, in sample point");
    int margin = 16;
    po.Register("margin", &margin, 
                "constraint for tdoa estimation");
    po.Read(argc, argv);

    if (po.NumArgs() != 2) {
        po.PrintUsage();
        exit(1);
    }
    std::string input_file = po.GetArg(1),
                       output_file = po.GetArg(2);
    WavReader wav_reader(input_file.c_str());

    printf("input file %s info: \n"
           "sample_rate %d \n"
           "channels %d \n"
           "bits_per_sample_ %d \n",
           input_file.c_str(),
           wav_reader.SampleRate(), 
           wav_reader.NumChannel(),
           wav_reader.BitsPerSample());

        int num_channel = wav_reader.NumChannel();
        int num_sample = wav_reader.NumSample();
        const float *pcm = wav_reader.Data();
        float *out_pcm = (float *)calloc(sizeof(float), num_sample);
            // char *out_pcm = (char *)calloc(sizeof(char), num_sample);
        int *tdoa = (int *)calloc(sizeof(int), num_channel);

    for (int i = 0; i < num_sample; i += beam_window) {
        int tdoa_window_size = (i + tdoa_window > num_sample) ? 
                            num_sample - i : tdoa_window;
        int beam_window_size = (i + beam_window > num_sample) ? 
                            num_sample - i : beam_window;
        assert(beam_window_size <= tdoa_window_size);
        // rearrange channel data
        float *data = (float *)calloc(sizeof(float), 
                                      tdoa_window_size * num_channel);
        float *beam_data = (float *)calloc(sizeof(float), 
                                      beam_window_size * num_channel);

        for (int j = 0; j < num_channel; j++) {
            for (int k = 0; k < tdoa_window_size; k++) {
                data[j * tdoa_window_size + k] = pcm[(i + k) * num_channel + j];
            }
            for (int k = 0; k < beam_window_size; k++) {
                beam_data[j * beam_window_size + k] = pcm[(i + k) * num_channel + j];
            }
        }
        // calc delay
        int tao = margin < tdoa_window_size / 2 ? margin : tdoa_window_size / 2;

        //改变增强声道
        GccPhatTdoa(data, num_channel, tdoa_window_size, 0, tao, tdoa);
        for (int j = 0; j < num_channel; j++) {
            printf("%d ", tdoa[j]);
        }
        printf("\n");

        DelayAndSum(beam_data, num_channel, beam_window_size, tdoa, out_pcm + i);
        

        free(data);
        free(beam_data);
    }

    // Write outfile
    WavWriter wav_writer( out_pcm, num_sample, 1,
                         wav_reader.SampleRate(), wav_reader.BitsPerSample());
    wav_writer.Write(output_file.c_str());

    int chl=0;
    float cov;
    cov, chl=covariance(pcm, out_pcm,num_channel, num_sample);
    cout<<chl<<' '<<cov<<endl;


    free(out_pcm);

    free(tdoa);
    
    // output the most similar channel

    return 0;
}
