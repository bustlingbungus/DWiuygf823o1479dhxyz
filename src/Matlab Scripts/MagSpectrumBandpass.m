clc;
inputfile = 'Enter_the_path_to_your_h5_file_here';
start_time = 120;
end_time = 240;
f_low = 0;
f_high = 200;
smoothing_window_size = 10;
gen_figures = 1;
domain_min = 0;
domain_max = 1000;
range_min = 10000000;
range_max = 999999986991104;
data_min = 0.02;
data_max = 50;
data_scale = 1;

%% Input Validation

f_low = max(f_low, 0.5);
domain_min = max(domain_min, 0.01);
data_min = clip(data_min, domain_min, domain_max);
start_time = clip(start_time, 0, end_time);
range_min = min(range_min, range_max - 1);

if data_max == -1 % data should end at domain end
    data_max = domain_max;
else
    data_max = clip(data_max, domain_min, domain_max);
end
    

% combine range parameters into one variable each 
domain = [domain_min, domain_max];
range = [range_min, range_max];
data_range = [data_min, data_max];



%% Filename Generation

% find file extension
ext = strfind(inputfile, '.h5');
if isempty(ext) % input file is not .h5 format
    disp(['ERROR: ''', inputfile, ''' is not a .h5 file, aborting pipeline.']);
    return;
end


% create a string containing the settings used to generate the output figures
info_str = strcat('_time=', num2str(start_time), '-', num2str(end_time), 's_targetingFreq=', num2str(f_low), '-', num2str(f_high), 'Hz_domain=', num2str(domain_min), '-', num2str(domain_max), 'Hz_range=', num2str(range_min), '-', num2str(range_max), 'dB_dataShown=', num2str(data_min), '-', num2str(data_max), 'Hz');
% base file name, the original path (minus the extension) along with the settings info
savebase = strcat(inputfile(1:ext-1), info_str);

% generate filenames for all output files
fig1_save = strcat(savebase, '__MagnitudeSpectrumOfAllChannels.png');
fig2_save = strcat(savebase, '__AverageMagnitudeSpectrum.png');
fig3_save = strcat(savebase, '__AverageMagnitudeSpectrumWithRectangles.png');
fig4_save = strcat(savebase, '__SineWavesWithDifferentFrequencies.png');
mat_save = strcat(savebase, '__AverageMagnitudeSpectrumData.txt');
stat_save = strcat(savebase, '__CurveAreas.txt');



%% Data Reading

% read data from input file
data = McsHDF5.McsData(inputfile);

% matrix of ALL data collected
matrix_full = data.Recording{1, 1}.AnalogStream{1, 1}.ChannelData;


% calculate sampling frequency
duration = double(data.Recording{1, 1}.Duration / 10^6);
n_samples_full = size(matrix_full, 2);
sampling_frequency = double(n_samples_full / round(duration));

% matrix used for writing sampling frequency and other stats later
stat_matrix = [sprintf('Sampling Frequency: %d Hz', round(sampling_frequency))];


% separate duration of interest (start-end time) from the full matrix
x = round(start_time * sampling_frequency);
y = round(end_time * sampling_frequency);
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



%% Define some variables used for plotting data

% define a separate domain that will actually be used in plotting
% neccesarry because normally the domain will cause issue when plotting 0 on a log scale
plot_domain = freq_domain;
if (freq_domain(2) > 0)
    plot_domain(1) = freq_domain(2) / 2;
else
    plot_domain(1) = 1e-3;
end

% find the indices of the first and last data points plotted
[~, data_idx_low] = min(abs(freq_domain - data_range(1)));
[~, data_idx_high] = min(abs(freq_domain - data_range(2)));



%% Plot power spectrum for all channels

if gen_figures == 1
    figure('Visible', 'off'); clf;
    hold on;
    colors = hsv(n_channels); % unique color for each channel

    
    % plot power spectra
    for i = 1:n_channels
        % hide portions of the data by setting those values to NaN, which isn't plotted
        plot_vals = power_spectra(i, :);
        plot_vals(1:data_idx_low-1) = NaN;
        plot_vals(data_idx_high:length(plot_vals)) = NaN;
        plot(plot_domain, plot_vals, 'Color', colors(i, :));
    end

    % formatting plot
    title('Power Spectrum of All Channels');
    xlabel('Frequency (Hz)');
    xlim(domain);
    set(gca, 'XScale', 'log');
    ylabel('Power (AU)');
    ylim(range);
    set(gca, 'YScale', 'log');
    saveas(gcf, fig1_save);
    hold off;
end
    


%% Average Power spectrum

% compute the average power spectrum
avg_power = mean(power_spectra, 1);
% Smooth the average power spectrum using a moving average filter
avg_power_smoothed = movmean(avg_power, smoothing_window_size);


% plot the average power spectrum
if gen_figures == 1
    % hide portions of the data by setting those values to NaN, which isn't plotted
    plot_vals = avg_power_smoothed;
    plot_vals(1:data_idx_low-1) = NaN;
    plot_vals(data_idx_high:length(plot_vals)) = NaN;

    % formatting graph
    figure('Visible', 'off'); clf;
    hold on;
    plot(plot_domain, plot_vals, 'r');
    title('Average Power Spectrum');
    xlabel('Frequency (Hz)');
    xlim(domain);
    set(gca, 'XScale', 'log');
    ylabel('Power (AU)');
    ylim(range);
    set(gca, 'YScale', 'log');
    saveas(gcf, fig2_save);
end



%% Area under power spectrum curve

% define ranges for each frequency band
% delta = black, theta = yellow, alpha = magenta, beta = blue
integration_ranges = [ data_min, 4, 'k'; 4, 8, 'y'; 8, 12, 'm'; 12, 20, 'b'; ];

band_areas = [0, 0, 0, 0];

for i = 1:length(integration_ranges)
    r = [integration_ranges(i, 1), integration_ranges(i, 2)];

    % find the first and last indices in the desired range on the graph
    % this is accomplished by subtracting the desired start and end frequencies
    % from the frequency domain values. By taking min of abs, we find the index
    % closest to zero (the desired index along the x axis)
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

        if gen_figures == 1
            % create rectangle to help visualize integral
            x = (j-1)*step;
            if (x >= data_min && step > 0 && avg_power_smoothed(j) > range(1)) % ensure valid rectangle
                rectangle('Position', [x, range(1), step, avg_power_smoothed(j)-range(1)], 'EdgeColor', integration_ranges(i, 3));
            end
        end
    end

    % save calculated area
    band_areas(i) = approx_integral;
end

% format areas to be saved both as full values, and with 4 sig figs
areas = [
    "Delta area: " + num2str(band_areas(1));
    "Theta area: " + num2str(band_areas(2));
    "Alpha area: " + num2str(band_areas(3));
    "Beta area: " + num2str(band_areas(4));
    "Delta area: " + sprintf('%.4g', band_areas(1));
    "Theta area: " + sprintf('%.4g', band_areas(2));
    "Alpha area: " + sprintf('%.4g', band_areas(3));
    "Beta area: " + sprintf('%.4g', band_areas(4))
];
stat_matrix = [stat_matrix; areas];

if gen_figures == 1
    saveas(gcf, fig3_save);
    hold off;
end



%% Display and save data statistics

disp(stat_matrix);
writematrix(stat_matrix, stat_save);


% Save graph file as txt format
% This is useful to plot several graphs in the same plot, another code
% required code is named as: MagSpectum Same Graph. This code takes the txt
% graph files saved
% For it to work, the "Current folder" on the left side needs to show a
% file with the animal number.
writematrix(avg_power_smoothed, mat_save)



if gen_figures == 1
    % Signal after filtration Amplitude x time (Can select range of channels to look at)

    % Create a time vector time_vector with the same number of elements 
    % Adjust the sampling frequency as needed
    time_vector = linspace(0, (n_samples_reduced-1)/double(sampling_frequency), n_samples_reduced);

    % plot the signals over time for verification
    figure('Visible', 'off'); clf;
    hold on;

    for i = 1:10
        plot(time_vector, filtered_data(i, :));
    end

    % plot formatting
    xlabel('Time (s)');
    ylabel('Amplitude')
    title('Sine Waves with Different Frequencies');
    saveas(gcf, fig4_save);
    hold off;
end