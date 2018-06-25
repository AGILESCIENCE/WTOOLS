% Compare MET and MC

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

%% MET

fprintf('Compute thresholds and generate masks\n');

cts_w_met_th    = zeros(1, size(cts_w,3));
cts_w_met_mask  = zeros(size(cts_w));
for i = 1 : 1:length(s)
    
    % Data
    cur_d = cts_w(:,:,i);
    
    % Get MET threshold
    [cur_hist_data,cur_hist_bin] = hist(cur_d(:), 128);
    cts_w_met_th(i) = met(cur_hist_data, cur_hist_bin);
    
    % Bin image
    cur_mask = zeros(cts_sz(1), cts_sz(2));
    cur_mask(cts_w(:,:,i) > cts_w_met_th(i)) = 1;
    cts_w_met_mask(:,:,i) = cur_mask;
    
end

%% MC

load('fm3_cwt_bkg.mat', 'cts_w_mean', 'cts_w_std');

cts_w_mc_th    = cts_w_mean + 4*cts_w_std;
cts_w_mc_mask  = zeros(size(cts_w));
for i = 1 : 1:length(s)
    
    % Bin image
    cur_mask = zeros(cts_sz(1), cts_sz(2));
    cur_mask(cts_w(:,:,i) > cts_w_mc_th(:,:,i)) = 1;
    cts_w_mc_mask(:,:,i) = cur_mask;
    
end

%% Plot

x_lim = [73 80];
y_lim = [-3 5];

close all

fh = figure('Name', 'Count map', 'NumberTitle', 'Off');
colormap(jet(256))
imagesc(cts_x, cts_y, cts)
set(gca, 'XDir', 'reverse', 'YDir', 'normal')
colorbar
hold on
axis equal
xlabel('b [deg]')
ylabel('l [deg]')

fh = figure('Name', 'Count map and contours MET', 'NumberTitle', 'Off');

colormap(gray(256))
imagesc(cts_x,cts_y,cts)

hold on

dx = abs(cts_x(2)-cts_x(1));
dy = abs(cts_y(2)-cts_y(1));

for i = 1:1:length(s)

    [B,L] = bwboundaries(cts_w_met_mask(:,:,i),8,'noholes');
    for k = 1:length(B)
        boundary = B{k};
        plot(cts_x(1) - boundary(:,2)*dx, cts_y(1) + boundary(:,1)*dy, 'r', 'LineWidth', 1)
    end

end    
   
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

fh = figure('Name', 'Count map and contours MET', 'NumberTitle', 'Off');

sel_s_id = 1:length(s);

sel_pix_mask = zeros(size(cts));

for i = sel_s_id(end:-1:1)
    
    sel_pix_id = find(cts_w_met_mask(:,:,i)>0);
    
    sel_pix_mask(sel_pix_id) = length(s)-i+1;
    
end

colormap(jet(256))
imagesc(cts_x,cts_y,sel_pix_mask);

hold on

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

fh = figure('Name', 'Count map and contours MC', 'NumberTitle', 'Off');

colormap(gray(256))
imagesc(cts_x,cts_y,cts)

hold on

dx = abs(cts_x(2)-cts_x(1));
dy = abs(cts_y(2)-cts_y(1));

for i = 1:1:length(s)

    [B,L] = bwboundaries(cts_w_mc_mask(:,:,i),8,'noholes');
    for k = 1:length(B)
        boundary = B{k};
        plot(cts_x(1) - boundary(:,2)*dx, cts_y(1) + boundary(:,1)*dy, 'r', 'LineWidth', 1)
    end

end    

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

fh = figure('Name', 'Count map and contours MC', 'NumberTitle', 'Off');

sel_s_id = 1:length(s);

sel_pix_mask = zeros(size(cts));

for i = sel_s_id(end:-1:1)
    
    sel_pix_id = find(cts_w_mc_mask(:,:,i)>0);
    
    sel_pix_mask(sel_pix_id) = length(s)-i+1;
    
end

colormap(jet(256))
imagesc(cts_x,cts_y,sel_pix_mask);

hold on

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

fh = figure('Name', 'Contours MET vs MC', 'NumberTitle', 'Off');

hold on

for i = 1:1:length(s)

    [B,L] = bwboundaries(cts_w_met_mask(:,:,i),8,'noholes');
    for k = 1:length(B)
        boundary = B{k};
        plot(cts_x(1) - boundary(:,2)*dx, cts_y(1) + boundary(:,1)*dy, 'r', 'LineWidth', 1)
    end
    
    [B,L] = bwboundaries(cts_w_mc_mask(:,:,i),8,'noholes');
    for k = 1:length(B)
        boundary = B{k};
        plot(cts_x(1) - boundary(:,2)*dx, cts_y(1) + boundary(:,1)*dy, 'g', 'LineWidth', 1)
    end

end    
   
set(gca, 'XDir', 'reverse', 'YDir', 'normal')

axis equal
xlabel('b [deg]')
ylabel('l [deg]')
