%% User defined variables
% Width and height of the fringes
width = 1024;
height = 768;

% 0 - Horizontal
% 1 - Vertical
direction = 0;

% Pitches of the patterns. Found using optimal finder above
pitch1 = 60;
pitch2 = 96; % They are ordered like this so 
pitch3 = 90; % that pitch12 is smaller than pitch13
%pitch1 = 90;
%pitch2 = 102;
%pitch3 = 135;

%% Generate Ideal Patterns
fringe1 = Gen3PhasePattern(pitch1, width, height, direction);
fringe2 = Gen3PhasePattern(pitch2, width, height, direction);
fringe3 = Gen3PhasePattern(pitch3, width, height, direction);

%% Dither patterns
fringe1Dither = stuckiDithering(fringe1);
fringe2Dither = stuckiDithering(fringe2);
fringe3Dither = stuckiDithering(fringe3);

%% Output generated patterns
for channel = 1 : 3
    out = uint8(fringe1Dither(:,:,channel) * 255.0);
    imwrite(out, sprintf('ninefringe-patterns/%d-%d.png', pitch1, channel));

    out = uint8(fringe2Dither(:,:,channel) * 255.0);
    imwrite(out, sprintf('ninefringe-patterns/%d-%d.png', pitch2, channel));
    
    out = uint8(fringe3Dither(:,:,channel) * 255.0);
    imwrite(out, sprintf('ninefringe-patterns/%d-%d.png', pitch3, channel));
end

%% Test phase unwrapping
pitch12 = (pitch1 * pitch2) / abs(pitch1 - pitch2);
pitch13 = (pitch1 * pitch3) / abs(pitch1 - pitch3);
pitch123 = (pitch12 * pitch13) / abs(pitch12 - pitch13);

phi1 = atan2((sqrt(3.0) .* (fringe1(:,:,1) - fringe1(:,:,3))), (2.0 .* fringe1(:,:,2)) - fringe1(:,:,1) - fringe1(:,:,3));
phi2 = atan2((sqrt(3.0) .* (fringe2(:,:,1) - fringe2(:,:,3))), (2.0 .* fringe2(:,:,2)) - fringe2(:,:,1) - fringe2(:,:,3));
phi3 = atan2((sqrt(3.0) .* (fringe3(:,:,1) - fringe3(:,:,3))), (2.0 .* fringe3(:,:,2)) - fringe3(:,:,1) - fringe3(:,:,3));

phi12 = mod(phi1 - phi2, 2.0 * pi);
phi13 = mod(phi1 - phi3, 2.0 * pi);
phi123 = mod(phi12 - phi13, 2.0 * pi);

% Remove the phase jump
%phi12 = atan2(sin(phi12),cos(phi12));

k13 = round( ( phi123 * ( pitch123 / pitch13 ) - phi13 ) / (2.0 * pi));
phase13 = phi13 + k13 * 2.0 * pi;

shift = .4; % Figure out why we have a shift
k3 = round( ( ( phase13 * ( pitch13 / pitch3 ) - phi3 ) / (2.0 * pi) ) - shift);
phase3 = phi3 + k3 * 2.0 * pi;

k = round( ( phase3 * ( pitch3 / pitch1 ) - phi1 ) / ( 2.0 * pi ) );
% This phase should be unwrapped
phase = phi1 + k * 2.0 * pi;
