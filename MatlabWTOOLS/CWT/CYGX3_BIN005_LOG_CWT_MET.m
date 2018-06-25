% Load CTS data and test LoG continuous wavelet transform
% Use the same kernel both for decomposition and reconstruction
% Select CWT coeffs usign Maximum Entropy Thresholding

clear all
close all
clc

addpath ../Core

%% Load electron count map

% Path to data
cts_path = '../../Data/CYGX3_BIN005/';

% File name
cts_fname = 'CYGX3_BIN005_POINTING.cts';

% Get file name from GUI
% [cts_fname,cts_path] = uigetfile('*.cts', 'Load FITS file', '../../Data/CYGX3_BIN005/');

% Load data and build x/y grid
[cts,cts_x,cts_y] = loadcts(fullfile(cts_path, cts_fname));
cts_sz = size(cts);

%% Decomposition

fprintf('Decomposition\n');

% Wavelet FT
csi_hat = @(wx,wy,sx,sy) (wx.^2+wy.^2).*exp(-sx^2*wx.^2/2-sy^2*wy.^2/2); 
csi_c   = 1;

% Build scales
s = scale('diadic', -5, 10, 3);

fprintf('Scales %d from %f to %f\n', length(s), min(s), max(s));

% Build pulsation plane
[wx,wy] = pulse2(cts_sz(1),cts_sz(2));

% Precompute data FT
cts_hat = fft2(cts);

% Compute CWT coeffs
cts_w = NaN*ones(cts_sz(1), cts_sz(2), length(s));
for i = 1:length(s)
    
    % Scale wavelet function
    cur_csi_hat = s(i)*csi_hat(s(i)*wx, s(i)*wy, 1, 1);
    
    % Compute coeffs
    cts_w(:,:,i) = ifft2(cts_hat.*conj(cur_csi_hat));
    
    fprintf('\t[%3d] scale %12.6f : coeff. min %10.4e max %10.4e\n', i, s(i), min(min(cts_w(:,:,i))), max(max(cts_w(:,:,i))));
    
end

%% Reconstruction

fprintf('Reconstruction\n');

cts_r = zeros(cts_sz(1), cts_sz(2));
for i = 1:length(s)
    
    % Scaled wavelet function
    cur_csi_hat = s(i)*csi_hat(s(i)*wx, s(i)*wy, 1, 1);
    
    % Coeff FT
    cts_w_hat = fft2(cts_w(:,:,i));
    
    % Add to reconstructed signal
    cts_r = cts_r + ifft2(cts_w_hat.*conj(cur_csi_hat))/(csi_c*s(i)^2); 
    
end

% Rescale
cts_min     = min(cts(:));
cts_max     = max(cts(:));
cts_r_min   = min(cts_r(:));
cts_r_max   = max(cts_r(:));
cts_r       = cts_min + (cts_r - cts_r_min)*(cts_max - cts_min)/(cts_r_max - cts_r_min);

% Error
cts_err        = cts - cts_r;
cts_err_l2     = sqrt(sum(cts_err(:).^2));
cts_err_l2_rel = cts_err_l2/sqrt(sum(cts(:).^2));

fprintf('Reconstruction error L2 %10.4e L2 rel. %10.4e\n', cts_err_l2, cts_err_l2_rel)

% Save data
% save('CYGX3_BIN005', 'cts_path', 'cts_fname', 'cts', 'cts_x', 'cts_y', 'cts_w') 

%% MET

fprintf('Compute thresholds\n');

cts_w_th    = zeros(1, size(cts_w,3));
cts_w_mask  = zeros(size(cts_w));
for i = 1 : 1:length(s)
    
    % Data
    cur_d = cts_w(:,:,i);
    
    % Get MET threshold
    [cur_hist_data,cur_hist_bin] = hist(cur_d(:), 128);
    cts_w_th(i) = met(cur_hist_data, cur_hist_bin);
    
    % Bin image
    cur_mask = zeros(cts_sz(1), cts_sz(2));
    cur_mask(cts_w(:,:,i) > cts_w_th(i)) = 1;
    cts_w_mask(:,:,i) = cur_mask;
    
    fprintf('\t[%3d] scale %12.6f : threshold %10.4e\n', i, s(i), cts_w_th(i));
    
end

%% Plot

close all

% fh = figure('Name', 'Count map', 'NumberTitle', 'Off');
% colormap(jet(256))
% imagesc(cts_x, cts_y, cts)
% set(gca, 'XDir', 'reverse', 'YDir', 'normal')
% colorbar
% hold on
% axis equal
% xlabel('b [deg]')
% ylabel('l [deg]')

% for i = 1 : length(s)
%    
%     figure_name = sprintf('Wavelet @ %10.4e', s(i));
%     
%     fh = figure('Name', figure_name, 'NumberTitle', 'Off');
%     colormap(jet(256))
%     imagesc(cts_x, cts_y, cts_w(:,:,i))
%     set(gca, 'XDir', 'reverse', 'YDir', 'normal')
%     
%     % set(gca, 'CLimMode', 'manual', 'CLim', [wcts_min wcts_max]);
%     
%     colorbar
%     hold on
%     axis equal
%     
% end

% fh = figure('Name', 'Reconstruction', 'NumberTitle', 'Off');
% colormap(jet(256))
% imagesc(cts_x, cts_y, cts_r)
% set(gca, 'XDir', 'reverse', 'YDir', 'normal')
% colorbar
% hold on
% axis equal

% fh = figure('Name', 'Error', 'NumberTitle', 'Off');
% colormap(jet(256))
% imagesc(cts_x, cts_y, cts_err)
% set(gca, 'XDir', 'reverse', 'YDir', 'normal')
% colorbar
% hold on
% axis equal

return

%% Iteractive plot of WT coeffs

global n x_lim y_lim

n       = [];
x_lim   = [70 85];
y_lim   = [-5 10];

close all

fh = figure('Name', 'Wavelet coeffs', 'NumberTitle', 'Off');
colormap(jet(256))
set(fh, 'KeyPressFcn', {@plotwt,cts_x,cts_y,cts_w,s});

return

%% Iteractive plot of MET segmented WT coeffs

global n x_lim y_lim

n       = [];
x_lim   = [70 85];
y_lim   = [-5 10];

close all

fh = figure('Name', 'Wavelet coeffs', 'NumberTitle', 'Off');
colormap(jet(256))
set(fh, 'KeyPressFcn', {@plotwt,cts_x,cts_y,cts_w.*cts_w_mask,s});

return

%% Make a GIF

close all

gif_file_name = 'test.gif';

[r,c,l] = size(cts_w);

fh = figure('Color', [1 1 1]);
colormap(jet(256))

for i = 1 : l
    
    imagesc(cts_x,cts_y,cts_w(:,:,i))
    set(gca, 'XDir', 'reverse', 'YDir', 'normal') 

    colorbar
    
    axis equal
    xlabel('b [deg]')
    ylabel('l [deg]')
    
    % title(sprintf('[%3d] scale %10.4f : coeff. min %10.4e max %10.4e', i, s(i), min(min(cts_w(:,:,i))), max(max(cts_w(:,:,i)))), 'FontWeight', 'bold')
    
    % title(sprintf('[%3d] scale %10.4f', i, s(i), 'FontWeight', 'bold'))
    
    drawnow
    
    frame = getframe(1);
    im = frame2im(frame);
    [imind,cm] = rgb2ind(im,256);
    if i == 1;
        imwrite(imind, cm, gif_file_name, 'gif', 'Loopcount', inf);
    else
        imwrite(imind, cm, gif_file_name, 'gif', 'WriteMode', 'append');
    end
    
    
end