function [TotalEnergy, ZeroCrossRate, Duration, SpectralPeaksMedian, SpectralFlux, SpectralFlatness, MeanPowerTime, SpectralSkewness, SpectralCentroid] = getFeatures(y, maxRows, Fs)

    [TotalEnergy, ZeroCrossRate, Duration] = getTimeFeatures(y, maxRows, Fs);

    [SpectralPeaksMedian, SpectralFlux, SpectralFlatness] = getFreqFeatures(y, maxRows, Fs);

    [MeanPowerTime, SpectralSkewness, SpectralCentroid] = getTimeFreqFeatures(y, maxRows, Fs);

end

function [TotalEnergy, ZeroCrossRate, Duration] = getTimeFeatures(y, maxRows, Fs)
    % Normalize the signal compressing it between -1 and 1
    min_y = min(y);
    max_y = max(y);
    y = (y - min_y) / (max_y - min_y);
    y = 2 * y - 1;
    % [rows, cols] are the original dimensions of y (there are #rows samples)
    [rows, ~] = size(y);

    % Split the audio signal in small frames in order to extract certain features
    % Frame size in seconds
    frameSize = 0.001;
    % Get number of samples per frame
    frameSamples = round(frameSize * Fs);
    % Calculate number of frames
    numFrames = floor(rows / frameSamples);

    % frameEnergy is an array with the energy value of every frame
    frameEnergy = getFrameEnergy(y, frameSamples, numFrames);
    energyThreshold = 0.1;

    % Find first index of the first frame with energy above threshold
    startFrame = find(frameEnergy > energyThreshold, 1);
    % Get the TIME index of the first frame with energy above threshold
    startSample = (startFrame - 1) * frameSamples + 1;

    % Find the index of the last frame with energy above threshold
    endFrame = find(frameEnergy > energyThreshold, 1, 'last');
    % Get the TIME index of the last frame with energy above threshold
    endSample = endFrame * frameSamples;  

    % Store the audio duration without silences in seconds
    Duration = (endSample - startSample) / Fs;
    % Store total energy
    TotalEnergy = sum(abs(y) .^ 2);
    % Store zero crossing rate
    ZeroCrossRate = zerocrossrate(y);
end

function [SpectralPeaksMedian, SpectralFlux, SpectralFlatness] = getFreqFeatures(y, maxRows, Fs)
    audioSignal = preProcessMeta2(y, maxRows, Fs);
    
    audioFFT = fft(audioSignal, 100000); % get the fourier series coefficients of the current sample of the current digit
    audioFFT = audioFFT(1:floor(length(audioFFT)/2)); % only take the first half of the data because the second half is symmetrical

    amplitudeSpectrum = abs(audioFFT); % get the magnitude of the complex fourier series coefficients
    amplitudeSpectrum = amplitudeSpectrum/length(audioFFT); % normalize by the number of samples
    amplitudeSpectrum = amplitudeSpectrum(1:3000); % only take the first 3000 frequencies
    
    % Spectral Peaks Median
    SpectralPeaksMedian = median(findpeaks(amplitudeSpectrum));
    
    % Spectral Flux
    SpectralFlux = sum(abs(diff(amplitudeSpectrum)), 1);
    
    % Spectral Flatness
    SpectralFlatness = geomean(amplitudeSpectrum) / mean(amplitudeSpectrum);
end

function [MeanPowerTime, SpectralSkewness, SpectralCentroid] = getTimeFreqFeatures(y, maxRows, Fs)
    audioSignal = preProcessMeta3(y, maxRows, Fs);
    Fs = 48000;
    maxTimeWindows = 120;
    % Spectrogram parameters
    wCoeff = 0.0032;
    windowSize = round(wCoeff * Fs);
    overlap = round(wCoeff * 0.5 * Fs);
    nfft = 2^nextpow2(windowSize);

    lastNonZero = find(audioSignal ~= 0, 1, 'last');
    trimmedAudioSignal = audioSignal(1:lastNonZero);

    % Get the STFT
    [s, f, t] = spectrogram(trimmedAudioSignal, hamming(windowSize), overlap, nfft, Fs, 'yaxis');

    powerSpectrum = abs(s) .^ 2;
    powerSpectrum = abs(10 * log10(powerSpectrum)); % Apply log scale

    % Mean Power per Time Window (Windows 40 - 50)
    MeanPowerTime = mean(powerSpectrum, 1);
    MeanPowerTime = mean(MeanPowerTime(40:50), 2);
    
    % Spectral Centroid
    SpectralCentroid = sum(f .* mean(abs(s), 2)) / sum(mean(abs(s), 2));

    % Spectral Skewness
    SpectralSkewness = sum((f - SpectralCentroid) .^ 3 .* mean(abs(s), 2)) / sum(mean(abs(s), 2));
end

