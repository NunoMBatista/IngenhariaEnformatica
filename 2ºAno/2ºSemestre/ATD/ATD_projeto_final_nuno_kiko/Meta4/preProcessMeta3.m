function [processedAudioSignal] = preProcessMeta3(y, maxRows, Fs)
    %create audio signals cell array

    % Normalize the signal based on the maximum amplitude
    min_y = min(y);
    max_y = max(y);
    %y = (y - min_y) / (max_y - min_y);
    %y = 2 * y - 1;
    y = y ./ max_y;
    
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

    % Trim y in order to remove low energy values
    y = y(startSample:end);
    % Add silence to the end of y
    concatY = zeros(maxRows - length(y), 1);
    y = [y; concatY];

    processedAudioSignal = y;   
end


