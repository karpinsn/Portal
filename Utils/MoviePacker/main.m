outVideo = VideoWriter('Out.avi', 'Uncompressed AVI');
inVideo = VideoReader('Jane.avi');
open(outVideo);
writeVideo( outVideo, imread('74.png') );
writeVideo( outVideo, imread('79.png') );

for frameNum = 1 : inVideo.NumberOfFrames
    frame = read(inVideo, frameNum);
    writeVideo( outVideo, frame );
end
close(outVideo);