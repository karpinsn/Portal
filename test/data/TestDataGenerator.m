%% Constants
pitch1 = 30;
pitch2 = 39;
pitch12 = (pitch1 * pitch2) / abs(pitch1 - pitch2);
width = 256;
height = 256;
rightSide = false;

%% Fringe Generation
x = 1 : width;
r = (1.0 - cos((2.0 * pi) * (x / pitch1) - (2.0*pi/3.0))) * .5;
g = (1.0 - cos((2.0 * pi) * (x / pitch1))) * .5;
b = (1.0 - cos((2.0 * pi) * (x / pitch1) + (2.0*pi/3.0))) * .5;

fringe1(:,:,1) = ones(height, 1) * r;
fringe1(:,:,2) = ones(height, 1) * g;
fringe1(:,:,3) = ones(height, 1) * b;

r = (1.0 - cos((2.0 * pi) * (x / pitch2) - (2.0*pi/3.0))) * .5;
g = (1.0 - cos((2.0 * pi) * (x / pitch2))) * .5;
b = (1.0 - cos((2.0 * pi) * (x / pitch2) + (2.0*pi/3.0))) * .5;

fringe2(:,:,1) = ones(height, 1) * r;
fringe2(:,:,2) = ones(height, 1) * g;
fringe2(:,:,3) = ones(height, 1) * b;

%% Phase Wrapping
phi1 = atan2((sqrt(3.0) .* (fringe1(:,:,1) - fringe1(:,:,3))), (2.0 .* fringe1(:,:,2)) - fringe1(:,:,1) - fringe1(:,:,3));
phi2 = atan2((sqrt(3.0) .* (fringe2(:,:,1) - fringe2(:,:,3))), (2.0 .* fringe2(:,:,2)) - fringe2(:,:,1) - fringe2(:,:,3));
phi12 = mod(phi1 - phi2, 2.0 * pi);

I(:,:,1) = sin(phi1);
I(:,:,2) = cos(phi1);
I(:,:,3) = sin(phi2);
I(:,:,4) = cos(phi2);
writePFM(I, 'WrappedPhaseComponents-30-39.pfm');

%% Phase Unwrapping
m = (2 * pi) / pitch12;
b1 = -1;
b2 = -5;
b3 = -6;
line1 = @(x) m * x + b1;
line2 = @(x) m * x + b2;
line3 = @(x) m * x + b3;

figure;
hold on;
plot(phi12(2,:));
plot(line1(x), 'k');
plot(line2(x), 'k');
plot(line3(x), 'k');

%% Unwrapping
unwrap = @(p, w) ( p < line2(w) && rightSide ) || ...
                 ( ...
                 ( ( p > line2(w) && p < line1(w) ) || ...
                   ( p < line2(w) && p < line3(w) ) ) ...
                   && ~rightSide );
               
for w = 1 : width
    for h = 1 : height
        phi12(h,w) = phi12(h,w) + unwrap(phi12(h,w), w) * 2 * pi;
    end
end

k = floor((phi12 * (pitch12/pitch1) - phi1) / (2.0 * pi));
phase = phi1 + k * 2.0 * pi;

outPhase(:,:,1) = phase;
outPhase(:,:,2) = phase;
outPhase(:,:,3) = phase;
outPhase(:,:,4) = 1.0;
writePFM(outPhase, 'UnwrappedPhase-30-39.pfm');
