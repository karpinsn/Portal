function [ phi1, phi2, phi12, mask ] = fringe2phase( fringe1, fringe2, gammaLevel, blackLevel )

% Wrap the phase
phi1 = atan2((sqrt(3.0) .* (fringe1(:,:,1) - fringe1(:,:,3))), (2.0 .* fringe1(:,:,2)) - fringe1(:,:,1) - fringe1(:,:,3));
phi2 = atan2((sqrt(3.0) .* (fringe2(:,:,1) - fringe2(:,:,3))), (2.0 .* fringe2(:,:,2)) - fringe2(:,:,1) - fringe2(:,:,3));

%{
h = fspecial('gaussian', [7 7], 7/3);
phi1C = imfilter(cos(phi1), h);
phi1S = imfilter(sin(phi1), h);
phi2C = imfilter(cos(phi2), h);
phi2S = imfilter(sin(phi2), h);

phi1 = atan2(phi1S, phi1C);
phi2 = atan2(phi2S, phi2C);
%}

% Calculate equivalent wavelengths
phi12 = mod(phi1 - phi2, 2.0 * pi);
gamma1 = sqrt( ( 2 .* fringe1(:,:,2) - fringe1(:,:,1) - fringe1(:,:,3) ).^2 + ...
    3 .* ( fringe1(:,:,1) - fringe1(:,:,3) ).^2 ) ./ (fringe1(:,:,1) + fringe1(:,:,2) + fringe1(:,:,3));

gamma2 = sqrt( ( 2 .* fringe2(:,:,2) - fringe2(:,:,1) - fringe2(:,:,3) ).^2 + ...
    3 .* ( fringe2(:,:,1) - fringe2(:,:,3) ).^2 ) ./ (fringe2(:,:,1) + fringe2(:,:,2) + fringe2(:,:,3));
gamma = min(gamma1, gamma2);

intensity1 = sqrt(((2.0 * fringe1(:,:,2) - fringe1(:,:,1) - fringe1(:,:,3)).^ 2.0) + 3.0 * ((fringe1(:,:,1) - fringe1(:,:,3)).^ 2.0));
intensity2 = sqrt(((2.0 * fringe2(:,:,2) - fringe2(:,:,1) - fringe2(:,:,3)).^ 2.0) + 3.0 * ((fringe2(:,:,1) - fringe2(:,:,3)).^ 2.0));
intensity = min(intensity1, intensity2);

mask = ~(gamma < gammaLevel | intensity < blackLevel);

end
