% Find the duration of the longest audio

scatter3dBest();

exampleID = 0;
digit = 0;

nCorrect = 0;
for digit = 0:9
    fprintf('\n\n\n>> %d <<\n\n', digit);
    for exampleID = 0:49
        nCorrect = nCorrect + filterDigitsLessFeatures(digit, exampleID);
    end
end

fprintf("Number of correct guesses: %d\n", nCorrect);
correctRate = nCorrect / 5;
fprintf("Correct guess rate using 3 features: %f\n", correctRate);

nCorrect = 0;
for digit = 0:9
    fprintf('\n\n\n>> %d <<\n\n', digit);
    for exampleID = 0:49
        nCorrect = nCorrect + filterDigits(digit, exampleID);
    end
end

fprintf("Number of correct guesses: %d\n", nCorrect);
correctRate = nCorrect / 5;
fprintf("Correct guess rate using 9 features: %f\n", correctRate);

function [correct] = filterDigits(digit, exampleID)
    correct = 0;
    % Load the features of every audio
    EveryTotalEnergy = csvread('TotalEnergy.csv');
    EveryZeroCrossRate = csvread('ZeroCrossRate.csv');
    EveryDuration = csvread('Duration.csv');
    EverySpectralPeaksMedian = csvread('SpectralPeaksMedian.csv');
    EverySpectralFlux = csvread('SpectralFlux.csv');
    EverySpectralFlatness = csvread('SpectralFlatness.csv');
    EveryMeanPowerTime = csvread('BestWindowPTB.csv');
    EverySpectralSkewness = csvread('SpectralSkewness.csv');
    EverySpectralCentroid = csvread('SpectralCentroid.csv');

    % Replace outliers in each row with NaN
    EveryTotalEnergy(isoutlier(EveryTotalEnergy, 2)) = NaN;
    EveryZeroCrossRate(isoutlier(EveryZeroCrossRate, 2)) = NaN;
    EveryDuration(isoutlier(EveryDuration, 2)) = NaN;
    EverySpectralPeaksMedian(isoutlier(EverySpectralPeaksMedian, 2)) = NaN;
    EverySpectralFlux(isoutlier(EverySpectralFlux, 2)) = NaN;
    EverySpectralFlatness(isoutlier(EverySpectralFlatness, 2)) = NaN;
    EveryMeanPowerTime(isoutlier(EveryMeanPowerTime, 2)) = NaN;
    EverySpectralSkewness(isoutlier(EverySpectralSkewness, 2)) = NaN;
    EverySpectralCentroid(isoutlier(EverySpectralCentroid, 2)) = NaN;

    maxRows = 0;
    for i = 0:9
        [y, ~] = audioread(sprintf("samples/%d_16_%d.wav", i, exampleID));
        [rows, ~] = size(y);
        if(maxRows < rows)
            maxRows = rows;
        end
    end
    [y, Fs] = audioread(sprintf("samples/%d_16_%d.wav", digit, exampleID));
    % Start by calculating every feature of the audio
    [TotalEnergy, ZeroCrossRate, Duration, SpectralPeaksMedian, SpectralFlux, SpectralFlatness, MeanPowerTime, SpectralSkewness, SpectralCentroid] = getFeatures(y, maxRows, Fs);

    currentlyPossible = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1];
    likely = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

    % Filter the audios by the features
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EveryTotalEnergy, TotalEnergy, currentlyPossible);      
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EveryZeroCrossRate, ZeroCrossRate, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EveryDuration, Duration, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EverySpectralPeaksMedian, SpectralPeaksMedian, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EverySpectralFlux, SpectralFlux, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EverySpectralFlatness, SpectralFlatness, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EveryMeanPowerTime, MeanPowerTime, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EverySpectralSkewness, SpectralSkewness, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EverySpectralCentroid, SpectralCentroid, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;

    fprintf("Sample %d (using 9 features) -> ", exampleID);
    % Print the possible digits
    npossible = sum(currentlyPossible);
    if(npossible == 1)
        fprintf('The digit is %d\n', digit);
        correct = 1;
    else
        fprintf('There are multiple possible digits ');
        guess = find((likely == max(likely))) - 1;
        if(length(guess) > 1)
            guess = guess(1);
        end
        fprintf('- Most Likely [%d]\n', guess);
        if(guess == digit)
            correct = 1;
        end
    end
    
end

function [updatedPossible, mostLikely, likelyFactor] = filterByFeature(EveryFeature, currentFeature, currentlyPossible)
    updatedPossible = currentlyPossible;
    minDifference = Inf;
    mostLikely = 1;
    
    likelyFactor = 0; 
    distancePerDigit = zeros(1, 10);

    for i = 1:10
        if(currentlyPossible(i) == 1)
            difference = abs(currentFeature - mean(EveryFeature(i)));
            if(difference < minDifference)
                minDifference = difference;
                mostLikely = i;

                % Calculate the likely factor
                % Add small constant to avoid division by zero
                likelyFactor = 1 / (difference + 0.0001);
                distancePerDigit(i) = difference;
            end
        end

        if(currentFeature > max(EveryFeature(i, :)) || currentFeature < min(EveryFeature(i, :)))
            updatedPossible(i) = 0;
        end
    end
    likelyFactor = 1 / (likelyFactor / sum(distancePerDigit));
end

function [correct] = filterDigitsLessFeatures(digit, exampleID)
    correct = 0;
    % Load the features of every audio
    EveryTotalEnergy = csvread('TotalEnergy.csv');  
    EveryZeroCrossRate = csvread('ZeroCrossRate.csv');   
    EverySpectralFlatness = csvread('SpectralFlatness.csv');
    
    % Replace outliers in each row with NaN
    EveryTotalEnergy(isoutlier(EveryTotalEnergy, 2)) = NaN;
    EveryZeroCrossRate(isoutlier(EveryZeroCrossRate, 2)) = NaN;
    EverySpectralFlatness(isoutlier(EverySpectralFlatness, 2)) = NaN;

    maxRows = 0;
    for i = 0:9
        [y, ~] = audioread(sprintf("samples/%d_16_%d.wav", i, exampleID));
        [rows, ~] = size(y);
        if(maxRows < rows)
            maxRows = rows;
        end
    end
    [y, Fs] = audioread(sprintf("samples/%d_16_%d.wav", digit, exampleID));
    % Start by calculating every feature of the audio
    [TotalEnergy, ZeroCrossRate, Duration, SpectralPeaksMedian, SpectralFlux, SpectralFlatness, MeanPowerTime, SpectralSkewness, SpectralCentroid] = getFeatures(y, maxRows, Fs);

    currentlyPossible = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1];
    likely = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

    % Filter the audios by the features
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EveryTotalEnergy, TotalEnergy, currentlyPossible);      
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EveryZeroCrossRate, ZeroCrossRate, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;
    [currentlyPossible, mostLikely, likelyFactor] = filterByFeature(EverySpectralFlatness, SpectralFlatness, currentlyPossible);
    likely(mostLikely) = likely(mostLikely) + likelyFactor;


    fprintf("Sample %d (using 3 features) -> ", exampleID);
    % Print the possible digits
    npossible = sum(currentlyPossible);
    if(npossible == 1)
        fprintf('The digit is %d\n', digit);
        correct = 1;
    else
        fprintf('There are multiple possible digits ');
        guess = find((likely == max(likely))) - 1;
        if(length(guess) > 1)
            guess = guess(1);
        end
        fprintf('- Most Likely [%d]\n', guess);
        if(guess == digit)
            correct = 1;
        end
    end
end

function [] = scatter3dBest()
    % Load the features of every audio
    EveryTotalEnergy = csvread('TotalEnergy.csv');  
    EveryZeroCrossRate = csvread('ZeroCrossRate.csv');   
    EverySpectralFlatness = csvread('SpectralFlatness.csv');

    % Create a 3D scatter plot
    figure;
    scatter3(EveryTotalEnergy, EveryZeroCrossRate, EverySpectralFlatness, 'filled');
    xlabel('Total Energy');
    ylabel('Zero Cross Rate');
    zlabel('Spectral Flatness');
    title('3D Scatter Plot of Audio Features');
    legend('Digit 0', 'Digit 1', 'Digit 2', 'Digit 3', 'Digit 4', 'Digit 5', 'Digit 6', 'Digit 7', 'Digit 8', 'Digit 9');
    grid on;

end
