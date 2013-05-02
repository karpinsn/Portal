size = 64;

amplitude = .9; % Should be somewhere between 0 - 1
sigma = .2; % Sigma for the gaussian distribution 

[X,Y]=meshgrid(linspace(0,1,size),linspace(0,1,size)); % generate a 2D grid of xy values
Z = amplitude * exp(-((X-0.5).^2+(Y-0.5).^2)/(2*sigma^2)); % generate the Gaussian 

figure;
imagesc(Z);

figure
mesh(X,Y,Z) % generate the perspective plot 
colormap([0 0 0]); % use black only 
xlabel('x (ph)'), ylabel('y (ph)'); 
set(gca,'ydir','reverse');

for c = 1 : 3
    blendImage(:,:,c) = ones(size,size) .* 255 .* Z;
end

blendImage = uint8(blendImage);
imwrite(blendImage, 'blend.png');