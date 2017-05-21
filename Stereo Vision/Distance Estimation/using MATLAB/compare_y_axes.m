function [ new_center1_sorted, new_center2_sorted ] = compare_y_axes( center1_sorted, center2_sorted)
% m and k represent the number of face in the images
[m, ~] = size(center1_sorted);
[k, ~] = size(center2_sorted);
new_center1_sorted = zeros(size(center1_sorted));
new_center2_sorted = zeros(size(center2_sorted));

% reference channel that includes min number of faces
z=0;
    if m<k
        % reference as channel1 (center1_sorted)
        for i=1:m % faces that are in the channel 1
            for j=1:k
                if (abs(center1_sorted(i,2) - center2_sorted(j,2))<=15)
                    z = z+1;    
                    new_center2_sorted(z,:) = center2_sorted(j,:);
                end
            end
        end
        new_center1_sorted = center1_sorted; % no change for center1
    
    else
        for i=1:k % faces that are in the channel 1
            for j=1:m
                if (abs(center2_sorted(i,2) - center1_sorted(j,2))<=15)
                    z = z+1;    
                    new_center1_sorted(z,:) = center1_sorted(j,:);
                end
            end
        end
        new_center2_sorted = center2_sorted; % no change for center1
    end
    
end

