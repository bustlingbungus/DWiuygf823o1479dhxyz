clc;
inputfile = 'path_to_input_h5_file';
output_location = 'path_to_output_folder';
start_time = 120;
end_time = 240;
f_low = 0;
f_high = 200;
smoothing_window_size = 10;
data_scale = 1;

%% Input Validation

f_low = max(f_low, 0.5);
start_time = clip(start_time, 0, end_time);


%% Filename Generation

% find recording's plate number
p_idx = strfind(inputfile, '39'); % all plate numbers start with 39
if isempty(p_idx)
    disp(['Could not detect plate number for ''', inputfile]);
    return;
end
% use index of '39' substring to find the rest of the plate number, convert to number
plate = inputfile(p_idx(1):p_idx(1)+4);

% generate filenames for all output files
% avg_mat_save = strcat(output_location, '\', plate, '_Average.txt');
% full_mat_save = strcat(output_location, '\', plate, '_Channels.txt');
areas_save = strcat(output_location, '\', plate, '_Areas.txt');



%% Data Reading
disp('reading data...')

% read data from input file
data = McsHDF5.McsData(inputfile);

% matrix of ALL data collected
matrix_full = data.Recording{1, 1}.AnalogStream{1, 1}.ChannelData;


% calculate sampling frequency
duration = double(data.Recording{1, 1}.Duration / 10^6);
n_samples_full = size(matrix_full, 2);
sampling_frequency = double(n_samples_full / round(duration));


% separate duration of interest (start-end time) from the full matrix
x = clip(round(start_time * sampling_frequency), 1, length(matrix_full(:,:)));
y = clip(round(end_time * sampling_frequency), x, length(matrix_full(:,:)));
reduced_data = matrix_full(:, x:y);



%% Sampling Statistics

% stats for reduced matrix
n_channels = size(reduced_data, 1); % number of channels recorded
n_samples_reduced = size(reduced_data, 2);
n_samples = n_samples_reduced; % Length of signal (number of samples)
s_period = 1 / sampling_frequency; % Sampling period

% if sample count is odd, make it even
if mod(n_samples, 2) == 1
    n_samples = n_samples - 1;
end



%% Signal Filtering
disp('filtering signal...')

% order is half the true filtering order, since `butter` doubles
% filter order for bandpass designs.
order = 2;

filtered_data = zeros(size(reduced_data));

% permit frequencies between specified ranges
Wn = [f_low, f_high] * 2 * s_period;
% create butterworth band-pass filter
[b, a] = butter(order, Wn, 'bandpass');

% apply filter
for i = 1:size(reduced_data, 1)
    filtered_data(i, :) = filtfilt(b, a, reduced_data(i, :));
end



%% Fourier Transform and Frequency Domain

% frequency domain for x axes
freq_domain = sampling_frequency * (0:(n_samples/2)) / n_samples;

% calculate fourier transforms
power_spectra = zeros(n_channels, n_samples/2 + 1);
for i = 1:n_channels
    P = abs(fft(filtered_data(i, :))); % full spectrum
    % single sided spectrum, normalized by number of samples
    % we double the values of the single sided spectrum
    channel_magnitude = 2 * P(1:n_samples/2 + 1) / n_samples; 

    % square magnitude data to get a power, then scale data
    power_spectra(i, :) = (channel_magnitude.^2) * data_scale; 
end


%% Average Power spectrum
disp('calculating channel average...')

% compute the average power spectrum
avg_power = mean(power_spectra, 1);
% Smooth the average power spectrum using a moving average filter
avg_power_smoothed = movmean(avg_power, smoothing_window_size);


%% Area under power spectrum curve
disp('calculating areas...');

% define ranges for each frequency band
integration_ranges = [ 0, 4; 4, 8; 8, 12; 12, 20; ];

band_areas = [0, 0, 0, 0];

for i = 1:length(integration_ranges)
    r = [integration_ranges(i, 1), integration_ranges(i, 2)];

    % find the first and last indices in the desired range on the graph
    [~, start_idx] = min(abs(freq_domain - r(1)));
    [~, end_idx] = min(abs(freq_domain - r(2)));
    % subtract 1 from end so that last rectangle stops AT end idx
    end_idx = end_idx - 1;

    n_pts = end_idx - start_idx; % the number of indices (number of rectangles)
    dist = abs(freq_domain(end_idx) - freq_domain(start_idx)); % distance between start and end indices
    step = dist / n_pts; % width of rectangles is the total distance over number of rectangles
    
    approx_integral = 0;

    for j = start_idx:end_idx
        % find area of current rectangle, and add to integral
        area = step * avg_power_smoothed(j);
        approx_integral = approx_integral + area;

    end

    % save calculated area
    band_areas(i) = approx_integral;
end


% Save areas as txt format
disp('saving results...')
writematrix(band_areas, areas_save);

disp('Pipeline complete.')