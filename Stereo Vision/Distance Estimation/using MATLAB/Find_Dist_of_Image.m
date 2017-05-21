% stereo vision example
% centers of the detected face are used to compute the distance
% through from reference point 
% below 'compare_y_axes' subfunction for if one more face in the frames and in the 
% condition of inequality
clc
close all

current_folder=pwd; 
path1 = '\test images\cam1_2.png';
path2 = '\test images\cam2_2.png';

img1 = imread(sprintf('%s%s',current_folder,path1));
img2 = imread(sprintf('%s%s',current_folder,path2));

load('stereoParams6x9.mat');
% [I1,I2] = rectifyStereoImages(img1,img2, stereoParams);
I1 = undistortImage(img1,stereoParams6x9.CameraParameters1);
I2 = undistortImage(img2,stereoParams6x9.CameraParameters2);

detector_face = vision.CascadeObjectDetector;
% detector_face = vision.CascadeObjectDetector('MergeThreshold',8);
face1 = step(detector_face,I1);
face2 = step(detector_face,I2);

[number_of_faces1 , vector_elements_face1] = size(face1(:,:)); 
[number_of_faces2 , vector_elements_face2] = size(face2(:,:)); 

center1 = zeros(number_of_faces1,2); % stores the all center elements in the first image
center2 = zeros(number_of_faces2,2); % stores the all center elements in the second image
no_faces = min(number_of_faces1,number_of_faces2); % faces that in the two images

if (number_of_faces1 ~= 0 && number_of_faces2 ~=0 )
        figure(1)
        imshow(I1)
        hold on
        for i=1:number_of_faces1
                    x = face1(i,1);
                    y = face1(i,2);
                    width = face1(i,3);
                    height = face1(i,4);
                    x_center1  = x + width/2;
                    y_center1  = y + height/2;
                    center1(i,:) = [x_center1 y_center1];
                    rectangle('Position',face1(i,:),'LineWidth',2,'EdgeColor','r');   
        end
        hold off
        title('cam1')
        center1_sorted = sortrows(center1); % sorts the center1 according to the rows 

        figure(2)
        imshow(I2)
        hold on
        for i=1:number_of_faces2
                    x = face2(i,1);
                    y = face2(i,2);
                    width = face2(i,3);
                    height = face2(i,4);
                    x_center2  = x + width/2;
                    y_center2  = y + height/2 ;
                    center2(i,:) = [x_center2 y_center2];
                    rectangle('Position',face2(i,:),'LineWidth',2,'EdgeColor','r');   
        end
        hold off
        title('cam2')
        center2_sorted = sortrows(center2); % sorts the center1 according to the rows
        face1_sorted = sortrows(face1);
        face2_sorted = sortrows(face2); 
        
        % only number of faces are not equal and more than one
         if number_of_faces2 ~= number_of_faces1
             [center1_sorted, center2_sorted] = compare_y_axes(center1_sorted, center2_sorted);
         end
        
    point3d = zeros (no_faces,3);  
        for i=1:no_faces
               point3d(i,:) = triangulate(center1_sorted(i,:), center2_sorted(i,:), stereoParams6x9);
               distanceInMeters = norm(point3d(i,:))/1000;
               %Display the detected face and distance.
               distanceAsString = sprintf('%0.2f meters', distanceInMeters);
               I1 = insertObjectAnnotation(I1,'Circle',[center1_sorted(i,:) 20],distanceAsString,'FontSize',18);
               I2 = insertObjectAnnotation(I2,'Circle',[center2_sorted(i,:) 20], distanceAsString,'FontSize',18);
               I1 = insertShape(I1,'FilledCircle',[center1_sorted(i,:) 20]);
               I2 = insertShape(I2,'FilledCircle',[center2_sorted(i,:) 20]);
              
        end
end
figure(3)
imshowpair(I1, I2, 'montage');
