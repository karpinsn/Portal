function main
outVideo = VideoWriter('Out.avi', 'Uncompressed AVI');
inVideo = VideoReader('Jane.avi');
open(outVideo);

packframe(imread('74.png'), outVideo);
packframe(imread('79.png'), outVideo);
packframe(imread('74.png'), outVideo);
packframe(imread('79.png'), outVideo);
packframe(imread('74.png'), outVideo);
packframe(imread('79.png'), outVideo);
packframe(imread('74.png'), outVideo);
packframe(imread('79.png'), outVideo);

for frameNum = 1 : inVideo.NumberOfFrames
    frame = read(inVideo, frameNum);
    writeVideo( outVideo, frame );
end
close(outVideo);

end

function [] = packframe(frame, outVideo)
    writeVideo( outVideo, frame(:,:,1) );
    writeVideo( outVideo, frame(:,:,2) );
    writeVideo( outVideo, frame(:,:,3) );
end