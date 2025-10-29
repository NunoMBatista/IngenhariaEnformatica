function [] = scatterplotFeature(data, feature, plotTitle)
    % Initialize an array to hold all feature values and an array to hold the corresponding digit labels
    allFeatureValues = [];
    digitLabels = [];

    % Loop over each digit
    for digit = 1:10
        % Get the feature values for the current digit
        currentFeatureValues = cell2mat(data{digit});

        % Append the feature values to the array of all feature values
        allFeatureValues = [allFeatureValues; currentFeatureValues];

        % Append the current digit label to the array of digit labels
        digitLabels = [digitLabels; repmat(digit, length(currentFeatureValues), 1)];
    end

    % Create a scatter plot
    scatter(digitLabels, allFeatureValues)

    % Set the x-axis labels
    xticks(0:9)

    % Add a title and labels
    title(plotTitle)
    xlabel('Digit')
    ylabel(feature)
end