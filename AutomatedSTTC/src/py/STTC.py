import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import h5py
import sys
from Functions import *

def main() -> int:
    use_h5 = True
    file_path = sys.argv[1]
    save_location = sys.argv[2]
    time_step = 1e-06

    # Define the synchronicity window (delta_t)
    delta_t = 100000  # 50 or 100 ms as a typical value

    filter_neurons = True
    filter_threshold = 1  # Minimum number of spikes a neuron must have to be included in analysis
    
    if use_h5:
        file = h5py.File(file_path,'r+')
        mapping = {mat[0]:int(mat[2].decode("utf-8").split(' ')[-1]) for mat in file[f'/Data/Recording_0/TimeStampStream/Stream_0/InfoTimeStamp/'][:]}
        electrodes = file['/Data/Recording_0/TimeStampStream/Stream_0/'].keys()
        time_stamp_data = {}
        for ind, electrode in mapping.items():
            try:
                time_stamp_data[electrode] = list(file[f'/Data/Recording_0/TimeStampStream/Stream_0/TimeStampEntity_{ind}'][:][0])
            except KeyError:
                continue
        time_stamp_data = pd.DataFrame(dict([(k, pd.Series(v)) for k, v in time_stamp_data.items()]))
        spike_trains = time_stamp_data
    else:
        spike_trains = pd.read_excel(file_path,header=0)

    
    if filter_neurons:
        spike_trains = spike_trains.loc[:, spike_trains.count(0)>=filter_threshold]

        min_spike_time = spike_trains.min().min()
        max_spike_time = spike_trains.max().max()
        total_obs_time = int(max_spike_time - min_spike_time)

    df = pd.DataFrame(spike_trains)
    delta_t = 50000 # 50 or 100 ms as a typical value
    T = total_obs_time

    # Calculate the STTC matrix for all neuron pairs
    sttc_matrix = calculate_STTC_pairwise(df, delta_t, T)


    mask = np.triu(np.ones_like(sttc_matrix, dtype=bool))
    f, ax = plt.subplots(figsize=(10, 10))
    cmap = sns.diverging_palette(230, 20, as_cmap=True)
    sns.heatmap(sttc_matrix,
                mask=mask,
                xticklabels=spike_trains.columns,
                yticklabels=spike_trains.columns,
                cmap='viridis',
                vmin=0,
                vmax=1,
                square=True, linewidths=0, cbar_kws={"shrink": .5})
    plt.xticks(rotation=90)
    plt.yticks(rotation=0)
    plt.savefig(save_location)
    # plt.show()

    
    return 0


main()