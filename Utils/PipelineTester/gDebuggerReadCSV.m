function [texture] = gDebuggerReadCSV(filename, width, height, dataType)
%GDEBUGGERREADCSV Reads in a gDebugger CSV file
%     
%   Arguments:
%       filename    - Filename of the gDebugger CSV file
%       width       - Width of the image
%       height      - Height of the image
%       dataType    - DataType to be read in '%f' '%c'
%
%   Returns:
%       texture - The texture that was saved in the gDebugger CSV file
%

% Open the file for reading
fid = fopen(filename, 'r');

textformat = ['%*s', repmat(dataType, 1, width)];
textureData = textscan(fid, textformat, 'delimiter', ',', 'HeaderLines', 17);
textureMat = cell2mat(textureData);
texture = zeros(height, width, 4);

texture(:, :, 1) = textureMat(1:4:height*4, :);
texture(:, :, 2) = textureMat(2:4:height*4, :);
texture(:, :, 3) = textureMat(3:4:height*4, :);
texture(:, :, 4) = textureMat(4:4:height*4, :);

% Close out the file
fclose(fid);