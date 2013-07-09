%% Constants
width = 256;
height = 256;
radius = 1;

%% Object generation (a sphere)
x = ones(height, 1) * linspace(-1, 1, width);
y = linspace(1, -1, height)' * ones(1, width);
z = radius^2 - x.^2 - y.^2;
a = ones(height, width);
% Make sure the points are valid. Mask any that are not
mask = x.^2 + y.^2 + z.^2 > radius.^2;
x(mask) = 0;
y(mask) = 0;
z(mask) = 0;
a(mask) = 0;
I(:,:,1) = x;
I(:,:,2) = y;
I(:,:,3) = z;
I(:,:,4) = a;

%% Split the object into the left camera and the right camera
Ileft = I;
Iright = I;

Ileft(:, 200:width, :) = 0.0;
Iright(:, 1:56, :) = 0.0;

%% Output the resulting data
writePFM(Ileft, 'LeftCamCoordinates.pfm');
writePFM(Iright, 'RightCamCoordinates.pfm');