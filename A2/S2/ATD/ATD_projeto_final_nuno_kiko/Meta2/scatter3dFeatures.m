function [] = scatter3dFeatures(data, feature1, feature2, feature3, plotTitle)
    % Initialize arrays to hold all feature values and an array to hold the corresponding digit labels
    allFeatureValues1 = [];
    allFeatureValues2 = [];
    allFeatureValues3 = [];
    digitLabels = [];

    % Define a colormap for the 10 digits
    colors = jet(10);

    % Loop over each digit
    for digit = 1:10
        % Get the feature values for the current digit
        currentFeatureValues1 = cell2mat(data{digit}(:, feature1));
        currentFeatureValues2 = cell2mat(data{digit}(:, feature2));
        currentFeatureValues3 = cell2mat(data{digit}(:, feature3));

        % Append the feature values to the array of all feature values
        allFeatureValues1 = [allFeatureValues1; currentFeatureValues1];
        allFeatureValues2 = [allFeatureValues2; currentFeatureValues2];
        allFeatureValues3 = [allFeatureValues3; currentFeatureValues3];

        % Append the current digit label to the array of digit labels
        digitLabels = [digitLabels; repmat(digit, length(currentFeatureValues1), 1)];
    end

    % Create a 3D scatter plot
    figure
    scatter3(allFeatureValues1, allFeatureValues2, allFeatureValues3, 10, colors(digitLabels, :), 'filled')

    % Add a title and labels
    title(plotTitle)
    xlabel(feature1)
    ylabel(feature2)
    zlabel(feature3)

    % Add a colorbar and set its ticks and labels
    colorbar('Ticks', 1:10, 'TickLabels', 0:9)
end