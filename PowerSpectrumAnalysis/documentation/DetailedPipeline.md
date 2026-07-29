# Detailed Analysis Pipeline

## Parameter validation

We start by ensuring all input parameters are reasonable values. If we somehow accidentally put -1 for our minimum targeting frequency, it would be corrected to 0.5. If we accidentally made the start time greater than the end time, it would be corrected, and so on. 

## Filename generation

The analysis pipeline will create several output files for each input. Four figures and two text files are generated. The names of these files will be the name of the input .h5 file, followed by the parameters being used, followed by a description of the figure.

For example:

```
inputFileName_time=120-130s_targetingFreq=1-4Hz_domain=0-1000Hz_range=70-160dB_dataShown=0.2-50Hz.png
```

## Reading data

This is the step where we read data from the .h5 file, and collect some simple statistics.

Data is read using the `McsHDF5` add on for MATLAB, using the `McsHDF5.McsData` command. The full data is saved to a simpler matrix, by accessing the `Recording > AnalogStream > ChannelData` member of the full data. 

We also read the duration and number of samples form the full recording, and use them to calculate the sampling frequency of the dataset by the formula

$F_s=\frac{n}{d}$, where $F_s$ is sampling frequency (Hz), $n$ is the number of samples in the recording, and $d$ is the duration of the recording (s).

Because we are only interested in the data collected within a particular time interval, we take a slice of the full channel data matrix that represents the data within the time range of interest. 

## Sampling Statistics

With the data reduced to the time range of interest, we calculate the following statistics regarding the reduced dataset:

- The number of recording channels in the reduced dataset
- The number of samples in the reduced dataset
- The sampling period (inverse of sampling frequency)

Later in the pipeline, we will be converting the data into a fourier transform. Because the fourier transform is symmetric for real valued data, it is more efficient to compute half the fourier domain, with no effective data loss<sup>[citation needed]</sup>. Because of this, the number of samples should be even (so that it can be halved). So, we round the number of samples down to the nearest even integer.  

## Signal Filtering

Our data is filtered using a fourth order bandpass filter, from 0.5 - 200 Hz. This is achieved using MATLAB's `butter` command to create the bandpass filter (note the the `butter` command doubles the filtering order for bandpass designs).

Then, the bandpass filter is applied to each of the recording channels using the `filtfilt` command.

## Frequency Domain and Fourier Transform 

The next step is to find the Fourier transform of the data. The Fourier transform allows us to analyze the data as frequencies, and perform several operations quickly. 

The Fourier transform for each recording channel is calculated using the `fft` MATLAB command. When calculating the Fourier transforms, we normalize the data by dividing the Fourier transform by the number of channels. Since we are interested in the magnitude in the Fourier domain, we also find the absolute value of this data. And finally, due to the symmetry of the Fourier domain mentioned above, we only take half of the Fourier transform data, and double the values in the new single-sided spectrum. Essentially,

$Fourier Data_i=\frac{2*|fft(filterdata)_i|}{n}$, for $i = [1, \frac{n}{2}]$, where $n$ is the number of samples.

We then calculate the freqeuncy domain of the data. The frequency domain is basically the x axis of the data when represented as a fourier transform. So, we essentially need to create an array of values representing discrete frequencies along the x axis of the fourier transform. As mentioned before, because the fourier transform is symmetric, this x axis will have half as many values as the number of samples in the dataset. 

Each discrete value in the frequency domain is an evenly spaced frequency, where the first value, $f_0$ is 0, and the last, $f_\frac{n}{2}$, is $\frac{F_s}{2}$, where $F_s$ is the sampling frequency, and n is the even number of samples described above. 

This is accomplished by taking the set $x$, $[0, 1, 2, ..., \frac{n}{2}]$, and constructing the set $f$, where $f_i=\frac{F_s}{n}*x_i$. This will construct the set $f$ such that it has the characteristics of the frequency domain, described above.  