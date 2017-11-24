%fixed = dicomread('knee1.dcm');
%moving = dicomread('knee2.dcm');

%fixed = imread('org_mu1.png');
%moving = imread('org_trs.bmp');

%fixed = imread('mu1_1.bmp');
%moving = imread('trs_1.bmp');

%[optmizer, metric] = imregconfig('multimodal');
%movingRegisteredDefault = imregister(moving, fixed, 'affine', optmizer, metric);
%figure, imshowpair(movingRegisteredDefault, fixed, 'montage');
%title('A: Default registration');
%disp(optmizer);
%disp(metric);

%optmizer.MaximumIterations = 300;
%optmizer.InitialRadius = optmizer.InitialRadius/7.0;
%movingRegisteredAdjustedInitialRadius = imregister(moving, fixed, 'affine', optmizer, metric);
%figure, imshowpair(movingRegisteredAdjustedInitialRadius, fixed);
%figure, imshowpair(movingRegisteredAdjustedInitialRadius, fixed, 'montage');
%title('Adjusted InitialRadius');
%disp(optmizer);
%disp(metric);
%imwrite(movingRegisteredAdjustedInitialRadius,'out.bmp')





mu1names = dir('C:\Users\pcdalao\Desktop\mu1_crop\*.bmp')
trsnames = dir('C:\Users\pcdalao\Desktop\trs_crop\*.bmp')
for i=1:length(mu1names)
    
    fixed = imread(strcat('C:\Users\pcdalao\Desktop\mu1_crop\',mu1names(i).name))
    moving = imread(strcat('C:\Users\pcdalao\Desktop\trs_crop\',trsnames(i).name))
    [optmizer, metric] = imregconfig('multimodal');
    optmizer.MaximumIterations = 3000;
    optmizer.InitialRadius = optmizer.InitialRadius/14.0;
    movingRegisteredAdjustedInitialRadius = imregister(moving, fixed, 'affine', optmizer, metric);
    imwrite(movingRegisteredAdjustedInitialRadius, trsnames(i).name)

end



