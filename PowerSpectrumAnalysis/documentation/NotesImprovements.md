# Notes and Potential Improvements

## Potential Improvements For MATLAB Data Analysis

### Change delta frequency bound

The UChicago paper used 0.1 Hz as the lower bound of the delta frequency band<sup>[[1]](#suresh-j-mihailo-radojicic-pesce-ll-et-al-network-burst-activity-in-hippocampal-neuronal-cultures-the-role-of-synaptic-and-intrinsic-currents-journal-of-neurophysiology-201611563073-3089-doihttpsdoiorg101152jn009952015)</sup>, but we used 0.5 Hz. The exact frequencies included in the delta frequency band seem to vary a bit source to source<sup>citation needed</sup>. Some say the lower bound is 0.5 Hz<sup>citation needed</sup>, hence our decision to start at 0.5 Hz, but many say the lower bound for the delta band is 0.1 Hz<sup>citation needed</sup>.

### Change filtering function

The UChicago team used the `filter` function to filter their signals<sup>[[1]](#suresh-j-mihailo-radojicic-pesce-ll-et-al-network-burst-activity-in-hippocampal-neuronal-cultures-the-role-of-synaptic-and-intrinsic-currents-journal-of-neurophysiology-201611563073-3089-doihttpsdoiorg101152jn009952015)</sup>. We use the `filtfilt` command. There does not *seem* to be an issue with using `filtfilt`, but but it may be woth investigating the differences between the two commands.

### Add/Edit butterworth filters

For our filtering, we use one fourth order butterworth filter. 

## Notes

1. #### When performing a butterworth filter in MATLAB, the `butter` command will effectively double your filtering order for bandpass and bandstop designs<sup>[[2]](#mathworks-lowpass-butterworth-transfer-function-mathworkscom-published-2019-httpswwwmathworkscomhelpsignalrefbutterhtml)</sup>.
    1. #### The UChicago team reports using one first order and one second order butterworth filter, implemented using the `butter` command in MATLAB<sup>[[1]](#suresh-j-mihailo-radojicic-pesce-ll-et-al-network-burst-activity-in-hippocampal-neuronal-cultures-the-role-of-synaptic-and-intrinsic-currents-journal-of-neurophysiology-201611563073-3089-doihttpsdoiorg101152jn009952015)</sup>. The `butter` command in MATLAB only accepts **integer** arguments, and will effectively double the filtering order of the argument used for bandpass and bandstop designs, but not for other designs<sup>[[2]](#mathworks-lowpass-butterworth-transfer-function-mathworkscom-published-2019-httpswwwmathworkscomhelpsignalrefbutterhtml)</sup>. The UChicago team states their first order filter was a low pass, and the second order filter was a band pass<sup>[[1]](#suresh-j-mihailo-radojicic-pesce-ll-et-al-network-burst-activity-in-hippocampal-neuronal-cultures-the-role-of-synaptic-and-intrinsic-currents-journal-of-neurophysiology-201611563073-3089-doihttpsdoiorg101152jn009952015)</sup>.<br><br>Ostensibly, this means the UChicago MATLAB code contains something similar to this:<br>
    ``` MATLAB
    [low_b, low_a] = butter(1, lowpass_window, 'low'); % first order lowpass filter
    [band_b, band_a] = butter(1, bandpass_window, 'bandpass'); % second order bandpass filter
    ```
    2. However, the paper does not mention the doubling behavior of using the `butter` command for a bandpass design<sup>[[1]](#suresh-j-mihailo-radojicic-pesce-ll-et-al-network-burst-activity-in-hippocampal-neuronal-cultures-the-role-of-synaptic-and-intrinsic-currents-journal-of-neurophysiology-201611563073-3089-doihttpsdoiorg101152jn009952015)[[2]](#mathworks-lowpass-butterworth-transfer-function-mathworkscom-published-2019-httpswwwmathworkscomhelpsignalrefbutterhtml)</sup>. It is possible the UChicago team passed `2` as the first argument (which is more intuitive, after all), which would make the bandpass filter a fourth order butterworth filter. Because the paper does not provide the actual MATLAB code used, it cannot be known if this issue was caught by the research team. 

<!-- 2. I find [this](https://pmc.ncbi.nlm.nih.gov/articles/PMC4946605/) national library of medicine article to be a more readable version of the cited UChicago paper<sup>[[1]](#suresh-j-mihailo-radojicic-pesce-ll-et-al-network-burst-activity-in-hippocampal-neuronal-cultures-the-role-of-synaptic-and-intrinsic-currents-journal-of-neurophysiology-201611563073-3089-doihttpsdoiorg101152jn009952015)</sup>. -->

2. The UChicago paper does not share their data analysis code, but they do make broad allusions to the methods used, i.e. "The ﬁlters were deﬁned in MATLAB(MathWorks, Natick, MA) with the butter command, and signals were filtered with the filter command"<sup>[[1]](#suresh-j-mihailo-radojicic-pesce-ll-et-al-network-burst-activity-in-hippocampal-neuronal-cultures-the-role-of-synaptic-and-intrinsic-currents-journal-of-neurophysiology-201611563073-3089-doihttpsdoiorg101152jn009952015)</sup>.

## References

1. #### Suresh J, Mihailo Radojicic, Pesce LL, et al. Network burst activity in hippocampal neuronal cultures: the role of synaptic and intrinsic currents. Journal of Neurophysiology. 2016;115(6):3073-3089. doi:https://doi.org/10.1152/jn.00995.2015

2. #### Mathworks. Lowpass Butterworth Transfer Function. Mathworks.com. Published 2019. https://www.mathworks.com/help/signal/ref/butter.html