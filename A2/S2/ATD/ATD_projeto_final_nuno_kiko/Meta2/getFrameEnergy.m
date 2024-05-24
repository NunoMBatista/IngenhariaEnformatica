function [frameEnergy] = getFrameEnergy(y, frameSamples, numFrames)
    % Calculate frame energy, the sum of squares of the samples in the frame (not the integral because we are in the discrete domain)
    frameEnergy = zeros(numFrames, 1);
    % Iterate through every frame
    for j = 1:numFrames
        % The frame range is from the (j-1)th*frameSamples frame to the (j+1)th*frameSamples frame
        frame = y((j - 1)*frameSamples + 1:j*frameSamples);
        % Get the frame's energy
        frameEnergy(j) = sum(frame .^ 2);
    end
    % End frame energy calculation
end
