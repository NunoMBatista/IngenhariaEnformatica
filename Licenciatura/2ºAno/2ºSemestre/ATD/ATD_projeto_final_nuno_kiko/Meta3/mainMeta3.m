audioSignals = cell(50, 1); % Each one of the 50 samples will contain the audio signals of each one of the digits
Fs = 48000;

for i = 1:50
    audioSignals{i} = preProcess(i - 1); % get the audio signals of each one of the digits from the sample
end

audioSignalMedians = cell(10, 1);

wCoeff = 0.0032;
% Spectrogram parameters
windowSize = round(wCoeff * Fs); % Window size of 0.0064ms
overlap = round(wCoeff * 0.5 * Fs); % Overlap should be half the window size
nfft = 2^nextpow2(windowSize); % Number of points for the FFT

% Start plotting the spectrograms
figure;
for i = 1:10
    % Get the audio signal
    audioSignal = audioSignals{1}{i}; % Get the first example of each digit
    
    % Find the last non-zero element
    lastNonZero = find(audioSignal ~= 0, 1, 'last');
    
    % Trim the audio signal
    trimmedAudioSignal = audioSignal(1:lastNonZero);
    
    subplot(5, 2, i);
    [s, f, t] = spectrogram(trimmedAudioSignal, hamming(windowSize), overlap, nfft, Fs, 'yaxis'); % Get the spectrogram data

    % Plot the spectrogram in logarithmic scale
    imagesc(t*1000, f, 10*log10(abs(s)));

    axis xy;
    title(['Digit ' num2str(i - 1)]);

    % Label the axes
    xlabel('Time (ms)');
    ylabel('Frequency (Hz)');
    
    % Colorbar label
    cb = colorbar;
    ylabel(cb, 'Power/Frequency (dB/Hz)');
end

spectrogramFeatures = containers.Map();

lowTimeWindow = 1;
maxTimeWindows = 120;
maxFreqBands = ceil(nfft / 2) + 1;

meanPowerFreqBandDigit = cell(10, 1);
meanPowerTimeBandDigit = cell(10, 1);
peakPowerTimeBandDigit = cell(10, 1);
spectralFlatnessTimeBandDigit = cell(10, 1);
powerSTDTimeBandDigit = cell(10, 1);
spectralFluxTimeBandDigit = cell(10, 1);
spectralRollOffTimeBandDigit = cell(10, 1);
powerSTDFreqBandDigit = cell(10, 1);
spectralSlopesTimeBandDigit = cell(10, 1);

spectralCentroidDigit = zeros(10, 50);
spectralSkewnessDigit = zeros(10, 50);

% These matrices will have 50 * 10 rows, one for each sample of each digit and f or t columns, one for each frequency band or time window
% They will later be used to get 3d scatter plots
everyMeanPowerFreqBand = [];
everyMeanPowerTimeBand = [];
everyPeakPowerTimeBand = [];
everySpectralFlatnessTimeBand = [];
everyPowerSTDTimeBand = [];
everySpectralFluxTimeBand = [];
everySpectralRollOffTimeBand = [];
everyPowerSTDFreqBand = [];
everySpectralRollOffFreqBand = [];
everySpectralSlopesTimeBand = [];

for digit = 1:10
    % These matrices will have 50 rows, one for each sample, and f or t columns, one for each frequency band or time window
    % We will then calculate the mean of each column to get the mean of each frequency band or time window over the 50 samples
    % Preallocate matrices
    MPFB = zeros(50, maxFreqBands); % Mean Power per Frequency Band
    MPTB = zeros(50, maxTimeWindows); % Mean Power per Time Band
    PPTB = zeros(50, maxTimeWindows); % Peak Power per Time Band
    SFTB = zeros(50, maxTimeWindows); % Spectral Flatness per Time Band
    PSTDTB = zeros(50, maxTimeWindows); % Power STD per Time Band
    SFluxTB = zeros(50, maxTimeWindows); % Spectral Flux per Time Band
    SROTB = zeros(50, maxTimeWindows); % Spectral Roll Off per Time Band
    PSTDFB = zeros(50, maxFreqBands); % Power STD per Frequency Band
    SROFB = zeros(50, maxFreqBands); % Spectral Roll Off per Frequency Band
    SSTB = zeros(50, maxTimeWindows); % Spectral Slopes per Time Band

    for sample = 1:50
        % Find the last non-zero element
        lastNonZero = find(audioSignals{sample}{digit} ~= 0, 1, 'last');
        
        % Trim the audio signal
        trimmedAudioSignal = audioSignals{sample}{digit}(1:lastNonZero);
        
        % Get the STFT from each sample
        % s is a matrix containing the STFT of the signal (rows -> frequencies, columns -> time, values -> power) 
        % f is a vector containing the frequency values
        % t is a vector containing the time values
        [s, f, t] = spectrogram(trimmedAudioSignal, hamming(windowSize), overlap, nfft, Fs, 'yaxis');
        
        % Apply logarithmic scale to the power values
        powerSpectrum = abs(s) .^ 2;
        powerSpectrum = abs(10 * log10(powerSpectrum)); 
        
        % When appending to the overall matrices, we need to make sure that the number of time windows is the same for all samples
        
        % Calculate the mean power per frequency band of the current sample
        curMPFB = mean(powerSpectrum, 2)'; % Column Matrix, transposed to be a row matrix
        MPFB(sample, :) = curMPFB;
        
        % Calculate the mean power per time band of the current sample
        curMPTB = mean(powerSpectrum, 1); % Row Matrix

        MPTB(sample, :) = addSilence(curMPTB, maxTimeWindows);

        % Calculate the peak power per time band of the current sample
        curPPTB = max(powerSpectrum, [], 1); % Row Matrix
        PPTB(sample, :) = addSilence(curPPTB, maxTimeWindows);
        
        % Calculate the spectral flatness per time band of the current sample
        curSFTB = geomean(powerSpectrum) ./ mean(powerSpectrum); % Row Matrix
        SFTB(sample, :) = addSilence(curSFTB, maxTimeWindows);
        
        % Calculate the power STD per time band of the current sample
        curPSTDTB = std(powerSpectrum, 0, 1); % Row Matrix
        PSTDTB(sample, :) = addSilence(curPSTDTB, maxTimeWindows);
        
        % Calculate the spectral centroid of the sample
        spectralCentroidDigit(digit, sample) = sum(f .* mean(abs(s), 2)) / sum(mean(abs(s), 2));

        % Calculate the spectral skewness of the sample
        spectralSkewnessDigit(digit, sample) = sum((f - spectralCentroidDigit(digit, sample)).^3 .* mean(abs(s), 2)) / sum(mean(abs(s), 2));
        
        % Calculate the Spectral Flux
        curSFluxTB = zeros(1, size(powerSpectrum, 2));
        curSFluxTB(1) = 0;
        for frame = 2:size(powerSpectrum, 2)
            curSFluxTB(frame) = sum((powerSpectrum(:, frame) - powerSpectrum(:, frame - 1)).^2);
        end
        SFluxTB(sample, :) = addSilence(curSFluxTB, maxTimeWindows);

        % Calculate the Spectral Roll Off per Time Band
        curSROTB = zeros(1, size(powerSpectrum, 2));
        cumulativeSum = cumsum(powerSpectrum, 1);
        totalSum = sum(powerSpectrum, 1);
        for frame = 1:size(powerSpectrum, 2)
            [~, rollOffIndex] = min(abs(cumulativeSum(:, frame) - 0.85 * totalSum(frame)));
            curSROTB(frame) = f(rollOffIndex);
        end
        SROTB(sample, :) = addSilence(curSROTB, maxTimeWindows);

        % Calculate the Power STD per Frequency Band
        curPSTDFB = std(powerSpectrum, 0, 2)'; % Column Matrix, transposed to be a row matrix
        PSTDFB(sample, :) = curPSTDFB;

        % Calculate the Spectral Roll Off per Frequency Band
        curSROFB = zeros(1, size(powerSpectrum, 1));
        cumulativeSum = cumsum(powerSpectrum, 2);
        totalSum = sum(powerSpectrum, 2);
        for frame = 1:size(powerSpectrum, 1)
            [~, rollOffIndex] = min(abs(cumulativeSum(frame, :) - 0.85 * totalSum(frame)));
            curSROFB(frame) = t(rollOffIndex);
        end
        SROFB(sample, :) = curSROFB;
        
        % Calculate the Spectral Slopes per Time Band
        curSSTB = zeros(1, size(powerSpectrum, 2));
        for frame = 1:size(powerSpectrum, 2)
            curSSTB(frame) = sum(abs(diff(powerSpectrum(:, frame))));
        end
        SSTB(sample, :) = addSilence(curSSTB, maxTimeWindows);

    end
    
    everyMeanPowerFreqBand = [everyMeanPowerFreqBand; MPFB];
    everyMeanPowerTimeBand = [everyMeanPowerTimeBand; MPTB];
    everyPeakPowerTimeBand = [everyPeakPowerTimeBand; PPTB];
    everySpectralFlatnessTimeBand = [everySpectralFlatnessTimeBand; SFTB];
    everyPowerSTDTimeBand = [everyPowerSTDTimeBand; PSTDTB];
    everySpectralFluxTimeBand = [everySpectralFluxTimeBand; SFluxTB];
    everySpectralRollOffTimeBand = [everySpectralRollOffTimeBand; SROTB];
    everyPowerSTDFreqBand = [everyPowerSTDFreqBand; PSTDFB];
    everySpectralRollOffFreqBand = [everySpectralRollOffFreqBand; SROFB];
    everySpectralSlopesTimeBand = [everySpectralSlopesTimeBand; SSTB];


    % Turn the matrix into a vector by getting the mean of each column
    MPFB = mean(MPFB, 1);
    MPTB = mean(MPTB, 1);
    PPTB = mean(PPTB, 1);
    SFTB = mean(SFTB, 1);
    PSTDTB = mean(PSTDTB, 1);
    SFluxTB = mean(SFluxTB, 1);
    SROTB = mean(SROTB, 1);
    PSTDFB = mean(PSTDFB, 1);
    SROFB = mean(SROFB, 1);
    SSTB = mean(SSTB, 1);

    meanPowerFreqBandDigit{digit} = MPFB;
    meanPowerTimeBandDigit{digit} = MPTB;
    peakPowerTimeBandDigit{digit} = PPTB;
    spectralFlatnessTimeBandDigit{digit} = SFTB;    
    powerSTDTimeBandDigit{digit} = mean(PSTDTB, 1);
    spectralFluxTimeBandDigit{digit} = SFluxTB;
    spectralRollOffTimeBandDigit{digit} = SROTB;
    powerSTDFreqBandDigit{digit} = PSTDFB;
    spectralRollOffFreqBandDigit{digit} = SROFB;
    spectralSlopesTimeBandDigit{digit} = SSTB;
end

spectrogramFeatures('Mean Power per Frequency Band') = meanPowerFreqBandDigit;
spectrogramFeatures('Mean Power per Time Band') = meanPowerTimeBandDigit;
spectrogramFeatures('Peak Power per Time Band') = peakPowerTimeBandDigit;
spectrogramFeatures('Spectral Flatness per Time Band') = spectralFlatnessTimeBandDigit;
spectrogramFeatures('Power STD per Time Band') = powerSTDTimeBandDigit;
spectrogramFeatures('Spectral Flux per Time Band') = spectralFluxTimeBandDigit;
spectrogramFeatures('Spectral Roll-Off per Time Band') = spectralRollOffTimeBandDigit;
spectrogramFeatures('Power STD per Frequency Band') = powerSTDFreqBandDigit;
spectrogramFeatures('Spectral Roll-Off per Frequency Band') = spectralRollOffFreqBandDigit;
spectrogramFeatures('Spectral Slopes per Time Band') = spectralSlopesTimeBandDigit;

spectrogramFeatures('Spectral Centroid') = spectralCentroidDigit;
spectrogramFeatures('Spectral Skewness') = spectralSkewnessDigit;


featuresStrings = {
    'Mean Power per Frequency Band',
    'Mean Power per Time Band',
    'Peak Power per Time Band',
    'Spectral Flatness per Time Band',
    'Power STD per Time Band',
    'Spectral Flux per Time Band', 
    'Spectral Roll-Off per Time Band',
    'Power STD per Frequency Band',
    'Spectral Roll-Off per Frequency Band',
    'Spectral slopes per Time Band'
};

% Plotting of the Mean Power per Frequency Band
figure;
plot3DScatterPlot(everyMeanPowerFreqBand, 'Mean Power per Frequency Band 3D', 'Frequency Band', 'Sample/(Digit+1)', 'Power(dB)');
figure;
plot2Dwindow(meanPowerFreqBandDigit, 'Mean Power per Frequency Band 2D', 'Frequency Band', 'Power (dB)');
figure;
boxplotWindows(everyMeanPowerFreqBand, 'Mean Power in Different Frequency Bands', 9, lowTimeWindow, maxFreqBands, 'Digits', 'Power (dB)');

% Plotting of the Mean Power per Time Band
figure;
plot3DScatterPlot(everyMeanPowerTimeBand, 'Mean Power per Time Band 3D', 'Time Band', 'Sample/(Digit+1)', 'Power(dB)');
figure;
plot2Dwindow(meanPowerTimeBandDigit, 'Mean Power per Time Band 2D', 'Time Band', 'Power (dB)');
figure;
boxplotWindows(everyMeanPowerTimeBand, 'Mean Power in Different Time Bands', 9, lowTimeWindow, maxTimeWindows, 'Digits', 'Power (dB)');

% Plotting of the Peak Power per Time Band
figure;
plot3DScatterPlot(everyPeakPowerTimeBand, 'Peak Power per Time Band 3D', 'Time Band', 'Sample/(Digit+1)', 'Power(dB)');
figure;
plot2Dwindow(peakPowerTimeBandDigit, 'Peak Power per Time Band 2D', 'Time Band', 'Power (dB)');
figure;
boxplotWindows(everyPeakPowerTimeBand, 'Peak Power in Different Time Bands', 9, lowTimeWindow, maxTimeWindows, 'Digits', 'Power (dB)');

% Plotting of the Spectral Flatness per Time Band
figure;
plot3DScatterPlot(everySpectralFlatnessTimeBand, 'Spectral Flatness per Time Band 3D', 'Time Band', 'Sample/(Digit+1)', 'Spectral Flatness');
figure;
plot2Dwindow(spectralFlatnessTimeBandDigit, 'Spectral Flatness per Time Band 2D', 'Time Band', 'Spectral Flatness');
figure;
boxplotWindows(everySpectralFlatnessTimeBand, 'Spectral Flatness in Different Time Bands', 9, lowTimeWindow, maxTimeWindows, 'Digits', 'Spectral Flatness');

% Plotting of the Power STD per Time Band
figure;
plot3DScatterPlot(everyPowerSTDTimeBand, 'Power STD per Time Band 3D', 'Time Band', 'Sample/(Digit+1)', 'Power STD');
figure;
plot2Dwindow(powerSTDTimeBandDigit, 'Power STD per Time Band 2D', 'Time Band', 'Power STD');
figure;
boxplotWindows(everyPowerSTDTimeBand, 'Power STD in Different Time Bands', 9, lowTimeWindow, maxTimeWindows, 'Digits', 'Power STD');

% Plotting of the Spectral Flux per Time Band
figure;
plot3DScatterPlot(everySpectralFluxTimeBand, 'Spectral Flux per Time Band 3D', 'Time Band', 'Sample/(Digit+1)', 'Spectral Flux');
figure;
plot2Dwindow(spectralFluxTimeBandDigit, 'Spectral Flux per Time Band 2D', 'Time Band', 'Spectral Flux');
figure;
boxplotWindows(everySpectralFluxTimeBand, 'Spectral Flux in Different Time Bands', 9, lowTimeWindow, maxTimeWindows, 'Digits', 'Spectral Flux');

% Plotting of the Spectral Roll-Off per Time Band
figure;
plot3DScatterPlot(everySpectralRollOffTimeBand, 'Spectral Roll-Off per Time Band 3D', 'Time Band', 'Sample/(Digit+1)', 'Spectral Roll-Off');
figure;
plot2Dwindow(spectralRollOffTimeBandDigit, 'Spectral Roll-Off per Time Band 2D', 'Time Band', 'Spectral Roll-Off');
figure;
boxplotWindows(everySpectralRollOffTimeBand, 'Spectral Roll-Off in Different Time Bands', 9, lowTimeWindow, maxTimeWindows, 'Digits', 'Spectral Roll-Off');

% Plotting of the Power STD per Frequency Band
figure;
plot3DScatterPlot(everyPowerSTDFreqBand, 'Power STD per Frequency Band 3D', 'Frequency Band', 'Sample/(Digit+1)', 'Power STD');
figure;
plot2Dwindow(powerSTDFreqBandDigit, 'Power STD per Frequency Band 2D', 'Frequency Band', 'Power STD');
figure;
boxplotWindows(everyPowerSTDFreqBand, 'Power STD in Different Frequency Bands', 9, lowTimeWindow, maxFreqBands, 'Digits', 'Power STD');

% Plotting of the Spectral Slopes per Time Band
figure;
plot3DScatterPlot(everySpectralSlopesTimeBand, 'Spectral Slopes per Time Band 3D', 'Time Band', 'Sample/(Digit+1)', 'Spectral Slopes');
figure;
plot2Dwindow(spectralSlopesTimeBandDigit, 'Spectral Slopes per Time Band 2D', 'Time Band', 'Spectral Slopes');
figure;
boxplotWindows(everySpectralSlopesTimeBand, 'Spectral Slopes in Different Time Bands', 9, lowTimeWindow, maxTimeWindows, 'Digits', 'Spectral Slopes');

% Plotting of the Spectral Centroid
figure;
boxplotReducedFeature(spectralCentroidDigit, 'Spectral Centroid', 'Spectral Centroid');

% Plotting of the Spectral Skewness
figure;
boxplotReducedFeature(spectralSkewnessDigit, 'Spectral Skewness', 'Spectral Skewness');

% Plot the 3 best features
figure;
subplot(1, 3, 1);
bestWindowPTB = extractBestWindow(everyMeanPowerTimeBand, 40, 50, 1);
bestWindowPTB = cell2mat(bestWindowPTB);

boxplotReducedFeature(bestWindowPTB, 'Mean Power per Frequency Band', 'Mean Power per Frequency Band');
subplot(1, 3, 2);
boxplotReducedFeature(spectralSkewnessDigit, 'Spectral Skewness', 'Spectral Skewness');
subplot(1, 3, 3);
boxplotReducedFeature(spectralCentroidDigit, 'Spectral Centroid', 'Spectral Centroid');


% Make a 3d scatterplot of the best features
figure;
annotation('textbox', [0 0.9 1 0.1], 'String', ['3D scatter plot of the best features'], 'EdgeColor', 'none', 'HorizontalAlignment', 'center', 'FontSize', 15);
scatter3(bestWindowPTB, spectralSkewnessDigit, spectralCentroidDigit);
xlabel('Mean Power per Time Band');
ylabel('Spectral Skewness');
zlabel('Spectral Centroid');
legend('Digit 0', 'Digit 1', 'Digit 2', 'Digit 3', 'Digit 4', 'Digit 5', 'Digit 6', 'Digit 7', 'Digit 8', 'Digit 9');
title('3D scatter plot of the best features'); 

% Write each one of the features to a separate CSV file
csvwrite('BestWindowPTB.csv', bestWindowPTB);
csvwrite('SpectralSkewness.csv', spectralSkewnessDigit);
csvwrite('SpectralCentroid.csv', spectralCentroidDigit);

function boxplotWindows(feature, plotTitle, nWindows, low, high, xl, yl)
    k = 1;

    skips = floor((high - low) / nWindows);

    for wind = low:skips:high
        subplot(2, 5, k);
        k = k + 1;
        if wind + floor(high/nWindows) < high
            highestWindow = wind + floor(high/nWindows);
            pl = cell2mat(extractBestWindow(feature, wind, wind + floor(high/nWindows), 1));
        else
            highestWindow = high;
            pl = cell2mat(extractBestWindow(feature, wind, high, 1));
        end
        boxplotReducedFeature(pl, [num2str(wind) ' to ' num2str(highestWindow)], [num2str(wind) ' to ' num2str(highestWindow)]);
        xlabel(xl);
        ylabel(yl);
    end
    sgtitle(plotTitle);
end

function plot2Dwindow(feature, plotTitle, xl, yl)
    for j = 1:10
        data = feature{j}(:);
        % Plot meanPowerFreqBandDigit for each digit in the same 2d plot, diferentiating digits by color
        plot(data, 'DisplayName', ['Digit ' num2str(j - 1)]);
        hold on;
    end
    title(plotTitle);
    colormap(jet(10));
    legend('Location', 'Best');

    xlabel(xl);
    ylabel(yl);
end

function plot3DScatterPlot(data, plotTitle, xl, yl, zl)
    % Get the number of rows and columns
    [nRows, nCols] = size(data);

    % Create a meshgrid for the row and column indices
    [X, Y] = meshgrid(1:nCols, 1:nRows);

    % Flatten the matSrices
    X = X(:);
    Y = Y(:);
    Z = data(:);

    colors = jet(10);

    colorIndices = ceil(Y / 50);

    % Create the 3D scatter plot
    scatter3(X, Y, Z, 10, colors(colorIndices, :), 'filled');
    xlabel(xl);
    ylabel(yl);
    zlabel(zl);
    set(gca, 'FontSize', 20);
    title(plotTitle);
    grid on;

    hold on;
    for i = 1:10
        scatter3(nan, nan, nan, 10, colors(i, :), 'filled', 'DisplayName', ['Digit ' num2str(i - 1)]);
    end
    hold off;

    legend('Location', 'Best');
end

function [newData] = addSilence(data, maxTimeWindows)
    if size(data, 2) < maxTimeWindows
        newData = [data zeros(1, maxTimeWindows - size(data, 2))];
    else
        newData = data(1:maxTimeWindows);
    end
end

function [reducedFeatureDigit] = extractBestWindow(everyWindowFeatureDigit, low, high, reductionType)
    reducedFeatureDigit = cell(10, 50);
    % Reduction types
    % 1 -> Mean
    % 2 -> Median
    % 3 -> Max
    % 4 -> Standard Deviation

    for dig = 1:10
        for samp = 1:50
            if reductionType == 1
                %reducedFeatureDigit{dig, samp} = mean(everyWindowFeatureDigit(dig*samp, low:high), 2);
                reducedFeatureDigit{dig, samp} = mean(everyWindowFeatureDigit((dig-1)*50 + samp, low:high), 2);
            elseif reductionType == 2
                reducedFeatureDigit{dig, samp} = median(everyWindowFeatureDigit((dig-1)*50 + samp, low:high), 2);
            elseif reductionType == 3
                reducedFeatureDigit{dig, samp} = max(everyWindowFeatureDigit((dig-1)*50 + samp, low:high), 2);
            elseif reductionType == 4
                reducedFeatureDigit{dig, samp} = std(everyWindowFeatureDigit((dig-1)*50 + samp, low:high), 1, 2);
            end
        end
    end
end

function boxplotReducedFeature(reducedFeature, featureString, plotTitle)
    feature = reducedFeature';

    xLabels = arrayfun(@num2str, 0:9, 'UniformOutput', false);

    boxplot(feature, 'Labels', xLabels, 'Symbol', '');
 
    title(plotTitle);
end