% create checkerboard pattern  8x8
clc
close all
clear all
% this is not a unique solution. This algorithm works only when m/200
% gives even number like 3000/ 300 = 10
m = 3000;
n = 3000;
% size of the little square is 300
A = ones(m,n); % size of the matrix
I = mat2gray(A, [0 1]); % matrix to gray level between 0 to 1
class(I) % double type
ctr = 0; % to control 1 black and 1 zero square
for i = 299: m-299 % up to size of the first square
    for j = 299: n-299
          if (mod(i,300) == 0 && mod(j,300)==0)
                ctr = ctr+1 ;
                if(mod(ctr,2)==1)
                    for x = i: i+300 % size of the square will be black
                        for y = j: j+300
                            if (x <m-300 && y < m-300)% up to size of the first square
                                I(x,y) = 0;
                            end
                        end
                    end
                end   
          end
    end
end
imshow(I);
imwrite(I,'mypattern.png')
