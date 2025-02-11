% audioFeatures[x, y, z] is the feature z, of the (x-1)th sample of digit y-1
% Features: 
% 1 - Total energy
% 2 - Standard deviation
% 3 - Max amplitude
% 4 - Zero crossing rate
% 5 - Duration
audioFeatures = zeros(50, 10, 5);

% Start by plotting the first sample of each digit (id = 0)
figure(1);
audioFeatures(1, :, :) = getFeatures(1, 1);
sgtitle('Signal examples');
% i + 1 because MATLAB uses 1-based indexing but the audio file names use 0-based indexing
for i = 0:49
    audioFeatures(i+1, :, :) = getFeatures(i, 0);
end
audioFeatures;
% Scatter plot the 5 chosen audio features
figure(2);
featureStrings = {'Total Energy', 'Standard Deviation', 'Max amplitude', 'Zero Crossing Rate', 'Duration (seconds)'};
for feature = 1:5
    subplot(1, 5, feature);
    % Iterate through every digit
    for digit = 1:10
        % The repmat function is used to create a 1x50 vector with repeated values of the current digit
        % Plot every value of the current digit and the current feature in the x value given by the repmat array (always the same digit in this iteration)
        scatter(repmat(digit-1, 1, 50), audioFeatures(:, digit, feature));
        % Retain current plot
        hold on;
    end
    title(featureStrings{feature});
    xlabel('Digit');
    ylabel('Value');
    % Make the x axis go from 0 to 9
    xticks(0:9);
    hold off;
end
sgtitle('Extracted audio features');
figure(3);
% Select the 3 best features for digit discrimination
bestFeatures = [1, 4, 5];
for i = 1:length(bestFeatures)
    feature = bestFeatures(i);
    subplot(1, length(bestFeatures), i);
    % Prepare data for boxplot
    data = [];
    group = [];
    for digit = 1:10
        % The data vector stores the feature values and the group vector stores the corresponding digit
        % i.e. data(n) is the value of the current feature of the nth sample of digit group(n)
        data = [data; audioFeatures(:, digit, feature)];
        group = [group; repmat(digit, 50, 1)];
    end
    % Plot each one of the boxes associated with it's corresponding digit
    boxplot(data, group-1);
    title(featureStrings{bestFeatures(i)});
    xlabel('Digit');
    ylabel('Value');
    xticklabels(0:9);
end
sgtitle('Best features for digit differentiation');

% Make a 3d scatter plot of the best features
audioFeatures(:, :, bestFeatures(1))

figure(4);
scatter3(audioFeatures(:, :, bestFeatures(1)), audioFeatures(:, :, bestFeatures(2)), audioFeatures(:, :, bestFeatures(3)));
xlabel(featureStrings{bestFeatures(1)});
ylabel(featureStrings{bestFeatures(2)});
zlabel(featureStrings{bestFeatures(3)});
legend('Digit 0', 'Digit 1', 'Digit 2', 'Digit 3', 'Digit 4', 'Digit 5', 'Digit 6', 'Digit 7', 'Digit 8', 'Digit 9');
title('3D scatter plot of the best features');
% End of mainMeta2.m


% save the 3 best features in csv files
bestFeatures = [1, 4, 5];
for i = 1:length(bestFeatures)
    feature = bestFeatures(i);
    % Prepare data for csvwrite
    data = [];
    for digit = 1:10
        % The data matrix stores the feature values
        % i.e. data(n, m) is the value of the current feature of the nth sample of digit m
        data = [data; audioFeatures(:, digit, feature)'];
    end
    % Write each one of the features to a separate CSV file
    csvwrite(['feature' num2str(i) '.csv'], data);
end