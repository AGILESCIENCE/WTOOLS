% Load CTS data and test LoG continuous wavelet transform
% Use the same kernel both for decomposition and reconstruction
% Select CWT coeffs usign Maximum Entropy Thresholding

clear all
close all
clc

addpath ../Core

%% Load gamma count map

% Path to data
cts_path = '../../Data/FM3/';

% File name
cts_fname = 'FM3.119_ASDCe_I0023_B01.cts.fts';

% Get file name from GUI
% [cts_fname,cts_path] = uigetfile('*.cts', 'Load FITS file', '../../Data/CYGX3_BIN005/');

% Load data and build x/y grid
[cts,cts_x,cts_y] = loadcts(fullfile(cts_path, cts_fname));
cts_sz = size(cts);

cts_mean = mean(cts(:));

cts = cts - cts_mean;

%% Decomposition

fprintf('Decomposition\n');

% Wavelet FT
csi_hat = @(wx,wy) 2*pi*(wx.^2+wy.^2).*exp(-wx.^2/2-wy.^2/2); 
csi_c   = 4*pi^3;

% Build scales
s = scale('diadic', 0.5, 8, 3);

fprintf('Scales %d from %f to %f\n', length(s), min(s), max(s));

% Build pulsation plane
[wx,wy] = pulse2(cts_sz(1),cts_sz(2));

% Precompute data FT
cts_hat = fft2(cts);

% Compute CWT coeffs
cts_w = NaN*ones(cts_sz(1), cts_sz(2), length(s));
for i = 1:length(s)
    
    % Scale wavelet function
    cur_csi_hat = s(i)*csi_hat(s(i)*wx, s(i)*wy);
    
    % Compute coeffs
    cts_w(:,:,i) = ifft2(cts_hat.*conj(cur_csi_hat)/s(i));
    
    fprintf('\t[%3d] scale %12.6f : coeff. min %10.4e max %10.4e\n', i, s(i), min(min(cts_w(:,:,i))), max(max(cts_w(:,:,i))));
    
end

% Save data
% save('CYGX3_BIN005', 'cts_path', 'cts_fname', 'cts', 'cts_x', 'cts_y', 'cts_w') 

%% Load thresholds

fprintf('Load thresholds and generate masks\n');

load('fm3_cwt_bkg.mat', 'cts_w_mean', 'cts_w_std');

cts_w_th    = cts_w_mean + 4*cts_w_std;
cts_w_mask  = zeros(size(cts_w));
for i = 1 : 1:length(s)
    
    % Bin image
    cur_mask = zeros(cts_sz(1), cts_sz(2));
    cur_mask(cts_w(:,:,i) > cts_w_th(:,:,i)) = 1;
    cts_w_mask(:,:,i) = cur_mask;
    
    fprintf('\t[%3d] scale %12.6f : coeff. min %10.4e max %10.4e : back min %10.4e max %10.4e\n', i, s(i), ...
        min(min(cts_w(:,:,i))), ...
        max(max(cts_w(:,:,i))), ...
        min(min(cts_w_th(:,:,i))), ...
        max(max(cts_w_th(:,:,i))));
    
    
end

%% Load sources

load fm3_source_list 

%% Plot

x_lim = [73 80];
y_lim = [-3 5];

close all

fh = figure('Name', 'Count map', 'NumberTitle', 'Off');
colormap(jet(256))
imagesc(cts_x, cts_y, cts)
set(gca, 'XDir', 'reverse', 'YDir', 'normal')
% colorbar
hold on
if exist('x_range', 'var')
    xlim(x_lim)
end
if exist('y_range', 'var')
    ylim(y_lim)
end
axis equal
xlabel('b [deg]')
ylabel('l [deg]')

fh = figure('Name', 'Count map and contours', 'NumberTitle', 'Off');

colormap(gray(256))
imagesc(cts_x,cts_y,cts)

hold on

dx = abs(cts_x(2)-cts_x(1));
dy = abs(cts_y(2)-cts_y(1));

for i = 1:1:length(s)

    [B,L] = bwboundaries(cts_w_mask(:,:,i),8,'noholes');
    for k = 1:length(B)
        boundary = B{k};
        plot(cts_x(1) - boundary(:,2)*dx, cts_y(1) + boundary(:,1)*dy, 'r', 'LineWidth', 1)
    end

end    
   
list_sel_id = find(list_x >= min(cts_x(:)) & list_x <= max(cts_x(:)));

plot(list_x(list_sel_id), list_y(list_sel_id), 'gs')

set(gca, 'XDir', 'reverse', 'YDir', 'normal')

if exist('x_lim', 'var')
    xlim(x_lim)
end
if exist('y_lim', 'var')
    ylim(y_lim)
end

axis equal
xlabel('b [deg]')
ylabel('l [deg]')

fh = figure('Name', 'Count map and contours', 'NumberTitle', 'Off');

sel_s_id = 1:length(s);

sel_pix_mask = zeros(size(cts));

for i = sel_s_id(end:-1:1)
    
    sel_pix_id = find(cts_w_mask(:,:,i)>0);
    
    sel_pix_mask(sel_pix_id) = length(s)-i+1;
    
end

colormap(jet(256))
imagesc(cts_x,cts_y,sel_pix_mask);

hold on

list_sel_id = find(list_x >= min(cts_x(:)) & list_x <= max(cts_x(:)));

plot(list_x(list_sel_id), list_y(list_sel_id), 'gs')

set(gca, 'XDir', 'reverse', 'YDir', 'normal')

if exist('x_lim', 'var')
    xlim(x_lim)
end
if exist('y_lim', 'var')
    ylim(y_lim)
end

axis equal
xlabel('b [deg]')
ylabel('l [deg]')

for i = [1 5 10 15 20] % 1 : length(s)
   
    figure_name = sprintf('[%02d] Wf @ %10.4e [deg]', i, s(i));
    
    fh = figure('Name', figure_name, 'NumberTitle', 'Off');
    colormap(jet(256))
    imagesc(cts_x, cts_y, cts_w(:,:,i))
    set(gca, 'XDir', 'reverse', 'YDir', 'normal')
    
    % set(gca, 'CLimMode', 'manual', 'CLim', [wcts_min wcts_max]);
    
    colorbar
    hold on
    axis equal
    
    if exist('x_range', 'var')
    xlim(x_lim)
    end
    if exist('y_range', 'var')
        ylim(y_lim)
    end

    axis equal
    xlabel('b [deg]')
    ylabel('l [deg]')
    
end

return

%% Iteractive plot of WT coeffs

global n x_lim y_lim

n       = [];
% x_lim   = [70 85];
% y_lim   = [-6 6];

close all

fh = figure('Name', 'Wavelet coeffs', 'NumberTitle', 'Off');
colormap(jet(256))
set(fh, 'KeyPressFcn', {@plotwt,cts_x,cts_y,cts_w,s,list_x,list_y,list_name});

return

%% Iteractive plot of MET segmented WT coeffs

global n x_lim y_lim

n       = [];
% x_lim   = [70 85];
% y_lim   = [-6 6];

close all

fh = figure('Name', 'Wavelet coeffs', 'NumberTitle', 'Off');
colormap(jet(256))
set(fh, 'KeyPressFcn', {@plotwt,cts_x,cts_y,cts_w.*cts_w_mask,s,list_x,list_y,list_name});

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