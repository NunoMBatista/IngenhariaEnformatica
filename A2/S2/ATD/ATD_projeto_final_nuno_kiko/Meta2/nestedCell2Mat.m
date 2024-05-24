function [outputMatrix] = nestedCell2Mat(cellArray)
    % Preallocate the output matrix
    outputMatrix = zeros(length(cellArray), length(cellArray{1}));

    % Iterate through every cell of the input cell array
    for i = 1:length(cellArray)
        % Iterate through every cell of the current cell of the input cell array
        for j = 1:length(cellArray{i})
            % Store the value of the current cell of the input cell array in the output matrix
            outputMatrix(i, j) = cellArray{i}{j};
        end
    end
    outputMatrix';
end