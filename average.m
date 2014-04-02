clearvars

dir='C:\Users\Simon\Desktop\X-Ray\Spiral_scan\S1_fast\S1_fast_Rec-2\cross\averages\average';

im=double(imread([dir '8.bmp']));

for i=10:1310
    
    im=imadd(im,double(imread([dir int2str(i) '.bmp'])));%im+double(imread([dir int2str(i) '.bmp']));
    
end


im=uint8(imdivide(im,1000));%max(max(max(im))));%1000);%im/(200000-7);

imwrite(im,[dir 'average.bmp']);