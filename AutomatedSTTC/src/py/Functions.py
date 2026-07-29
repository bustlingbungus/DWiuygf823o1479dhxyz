import numpy as np
import pandas as pd

def compute_T(spikes, delta_t, T_total):
    spikes = np.sort(spikes)
    covered = 0
    start = max(spikes[0] - delta_t, 0)
    end = min(spikes[0] + delta_t, T_total)
    for spike in spikes[1:]:
        new_start = max(spike - delta_t, 0)
        new_end = min(spike + delta_t, T_total)
        if new_start <= end:
            end = max(end, new_end)
        else:
            covered += end - start
            start = new_start
            end = new_end
    covered += end - start
    return covered / T_total

def calculate_STTC(spikes_a, spikes_b, delta_t, T=100000):
    """
    Calculate Spike Time Tiling Coefficient between two spike trains.
    
    Parameters:
    -----------
    spikes_a : array-like
        Spike times for neuron A
    spikes_b : array-like
        Spike times for neuron B
    delta_t : float
        Time window
    T : float
        Total recording time
        
    Returns:
    --------
    float
        STTC value
    """
    if len(spikes_a) == 0 or len(spikes_b) == 0:
        return 0.0
    
    # Convert to numpy arrays for faster operations
    spikes_a = np.array(spikes_a)
    spikes_b = np.array(spikes_b)
    
    # Create a 2D matrix of time differences
    time_diff = np.abs(spikes_a[:, np.newaxis] - spikes_b[np.newaxis, :])
    
    # Find spikes within delta_t window
    within_window = time_diff <= delta_t
    
    # Calculate PA and PB (proportion of spikes from one train that fall within 
    # delta_t of any spike from the other train)
    Pa = np.sum(np.any(within_window, axis=1)) / len(spikes_a)
    Pb = np.sum(np.any(within_window, axis=0)) / len(spikes_b)
    
    # Calculate TA and TB (proportion of total recording time that falls within 
    # delta_t of any spike from the train)
    Ta = compute_T (spikes_a, delta_t, T)
    #np.sum(np.any(within_window, axis=1)) * delta_t / T
    Tb = compute_T (spikes_b, delta_t, T)
    #= np.sum(np.any(within_window, axis=0)) * delta_t / T
    
    # Calculate STTC
    if Pa == 1 and Tb == 1:
        part1 = 1
    else:
        part1 = (Pa - Tb) / (1 - Pa * Tb)
        
    if Pb == 1 and Ta == 1:
        part2 = 1
    else:
        part2 = (Pb - Ta) / (1 - Pb * Ta)
    
    STTC = 0.5 * (part1 + part2)
    
    return STTC

def calculate_STTC_from_df(df, delta_t, neuron_a='Neuron A', neuron_b='Neuron B', T=100000):
    """Wrapper function to extract spike times from a DataFrame and calculate STTC"""
    spikes_a = df[neuron_a].dropna().values
    spikes_b = df[neuron_b].dropna().values
    return calculate_STTC(spikes_a, spikes_b, delta_t, T)

def calculate_STTC_pairwise(df, delta_t, T):
    """
    Calculate STTC for all pairs of neurons in the DataFrame.
    
    Parameters:
    -----------
    df : pandas.DataFrame
        DataFrame where each column represents a neuron and contains spike times
    delta_t : float
        Time window
    T : float
        Total recording time
        
    Returns:
    --------
    pandas.DataFrame
        Matrix of STTC values for each neuron pair
    """
    neurons = df.columns
    n_neurons = len(neurons)
    
    # Pre-extract all spike trains to avoid repeated operations
    spike_trains = {neuron: df[neuron].dropna().values for neuron in neurons}
    
    # Initialize the result matrix
    sttc_matrix = pd.DataFrame(index=neurons, columns=neurons, dtype=float)
    
    # Calculate STTC for each pair
    for i, neuron_a in enumerate(neurons):
        # Set diagonal to 1 (neuron with itself)
        sttc_matrix.loc[neuron_a, neuron_a] = 1.0
        
        # Only compute for j > i to avoid redundant calculations
        for j, neuron_b in enumerate(neurons):
            if j > i:  # Only compute upper triangle
                spikes_a = spike_trains[neuron_a]
                spikes_b = spike_trains[neuron_b]
                
                # Calculate STTC for this pair
                sttc_value = calculate_STTC(spikes_a, spikes_b, delta_t, T)
                
                # Store the value in both positions (symmetric matrix)
                sttc_matrix.loc[neuron_a, neuron_b] = sttc_value
                sttc_matrix.loc[neuron_b, neuron_a] = sttc_value
            elif j < i:
                # Copy value from already computed position (lower triangle)
                sttc_matrix.loc[neuron_a, neuron_b] = sttc_matrix.loc[neuron_b, neuron_a]
    
    return sttc_matrix