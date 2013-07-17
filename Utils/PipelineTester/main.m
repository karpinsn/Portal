%% Constants
P1 = 54;
P2 = 60;
P12 = (P1 * P2) / abs(P1 - P2);

%% Fringe loading
leftFringe1 = imread('bin\Left1.png');
leftFringe2 = imread('bin\Left2.png');
rightFringe1 = imread('bin\Right1.png');
rightFringe2 = imread('bin\Right2.png');

[height, width, ~] = size(leftFringe1);

%% Phase wrapping
[l1, l2, l12, lmask] = fringe2phase( double(leftFringe1), double(leftFringe2), .10, .10 * 255);
[r1, r2, r12, rmask] = fringe2phase( double(rightFringe1), double(rightFringe2), .10, .10 * 255);

%% Mask phase
l1(0.0 == lmask) = 0.0;
l2(0.0 == lmask) = 0.0;
l12(0.0 == lmask) = 0.0;

r1(0.0 == rmask) = 0.0;
r2(0.0 == rmask) = 0.0;
r12(0.0 == rmask) = 0.0;

%% Components Generation
leftComponents(:,:,1) = sin(l1) .* lmask;
leftComponents(:,:,2) = cos(l1) .* lmask;
leftComponents(:,:,3) = sin(l2) .* lmask;
leftComponents(:,:,4) = cos(l2) .* lmask;

rightComponents(:,:,1) = sin(r1) .* rmask;
rightComponents(:,:,2) = cos(r1) .* rmask;
rightComponents(:,:,3) = sin(r2) .* rmask;
rightComponents(:,:,4) = cos(r2) .* rmask;

%% Save Components
writePFM(leftComponents, 'bin\LeftComponents.pfm');
writePFM(rightComponents, 'bin\RightComponents.pfm');

%% Component Check
portalLeftComponents = gDebuggerReadCSV('bin\LeftComponents.csv', 800, 600, '%f');
portalRightComponents = gDebuggerReadCSV('bin\RightComponents.csv', 800, 600, '%f');

leftComponentDelta = portalLeftComponents - leftComponents;
rightComponentDelta = portalRightComponents - rightComponents;

%% Component Smoothing
H = fspecial('gaussian', 7, 7/3);
leftComponents = imfilter(leftComponents, H);
rightComponents = imfilter(rightComponents, H);

%% Phase Recovery
l1 = atan2(leftComponents(:,:,1), leftComponents(:,:,2));
l2 = atan2(leftComponents(:,:,3), leftComponents(:,:,4));
l12 = mod( l1 - l2, 2*pi );

r1 = atan2(rightComponents(:,:,1), rightComponents(:,:,2));
r2 = atan2(rightComponents(:,:,3), rightComponents(:,:,4));
r12 = mod( r1 - r2, 2*pi );

%% Phase Unwrapping
leftM = 0.011635528346629;
leftB = -1.576614090968211;
leftUnwrap = @(x) leftM * x + leftB;

rightM = 0.010833078115827;
rightB = -1.218721288030523;
rightUnwrap = @(x) rightM * x + rightB;

for w = 1 : width
    for h = 1 : height
        l12(h,w) = l12(h,w) + (l12(h,w) < leftUnwrap(w)) * 2 * pi;
        r12(h,w) = r12(h,w) + (r12(h,w) < rightUnwrap(w)) * 2 * pi;
    end
end

% Unwrap phi1
kl = floor((l12 * (P12/P1) - l1) / (2.0 * pi));
kr = floor((r12 * (P12/P1) - r1) / (2.0 * pi));

% This phase should be unwrapped
phaseL = l1 + kl * 2.0 * pi;
phaseR = r1 + kr * 2.0 * pi;

% Now mask
phaseL(~lmask) = 0.0;
phaseR(~rmask) = 0.0;

%% Save Phase
phase(:,:,1) = phaseL;
phase(:,:,2) = phaseL;
phase(:,:,3) = phaseL;
phase(:,:,4) = lmask;
writePFM(phase, 'bin\LeftPhase.pfm');
phase(:,:,1) = phaseR;
phase(:,:,2) = phaseR;
phase(:,:,3) = phaseR;
phase(:,:,4) = rmask;
writePFM(phase, 'bin\RightPhase.pfm');

%% Phase Check
portalLeftPhase = gDebuggerReadCSV('bin\LeftPhase.csv', 800, 600, '%f');
portalRightPhase = gDebuggerReadCSV('bin\RightPhase.csv', 800, 600, '%f');
portalLeftPhase = portalLeftPhase(:,:,1);
portalRightPhase = portalRightPhase(:,:,1);

leftPhaseDelta = portalLeftPhase - phaseL;
rightPhaseDelta = portalRightPhase - phaseR;

%% Data loading
rightCoordinates = gDebuggerReadCSV('bin\RightCoords.csv', 800, 600, '%f');
leftCoordinates = gDebuggerReadCSV('bin\LeftCoords.csv', 800, 600, '%f');

%% Masking
rightMask = rightCoordinates(:,:,4) == 0.0;
leftMask = leftCoordinates(:,:,4) == 0.0;
for channel = 1 : 3
    temp = rightCoordinates(:,:,channel);
    temp(rightMask) = nan;
    rightCoordinates(:,:,channel) = temp;
    temp = leftCoordinates(:,:,channel);
    temp(leftMask) = nan;
    leftCoordinates(:,:,channel) = temp;
end

%% Remove mask channel
rightCoordinates = rightCoordinates(:,:,1:3);
leftCoordinates = leftCoordinates(:,:,1:3);

%% Plot the results
close all;
figure;
hold on;
%surf(rightCoordinates(:,:,1),rightCoordinates(:,:,2),rightCoordinates(:,:,3), 'EdgeColor', 'none', 'FaceLighting', 'phong', 'FaceColor', 'interp');
surf(leftCoordinates(:,:,1),leftCoordinates(:,:,2),leftCoordinates(:,:,3), 'EdgeColor', 'none', 'FaceLighting', 'phong', 'FaceColor', 'interp');

min = -100;
max = 200;

axis([min, max, min, max, min, max]);

%% ICP???
left = [reshape(leftCoordinates(:,:,1),1,[]);
        reshape(leftCoordinates(:,:,2),1,[]);
        reshape(leftCoordinates(:,:,3),1,[])];

right = [reshape(rightCoordinates(:,:,1),1,[]);
         reshape(rightCoordinates(:,:,2),1,[]);
         reshape(rightCoordinates(:,:,3),1,[])];
     
% Remove nan
left(:, any(isnan(left),1))=[];
right(:, any(isnan(right),1))=[];
     
%%
[R, T] = icp(left, right);

%%
% R = [0.929811815573469,-0.279105209839177,-0.239896372337286;0.170914331480944,0.904741498966950,-0.390168055254973;0.325942140439645,0.321781139732670,0.888942416131635];
% T = [8.971034618824099;39.721795479852700;-50.343384949566410];

correctedRight = R * right + (T * ones(1, 47782));

close all;
figure;
hold on;
%scatter3(correctedRight(1,:),correctedRight(2,:),correctedRight(3,:), '.');
scatter3(left(1,:),left(2,:),left(3,:), '.');
axis([min, max, min, max, min, max]);