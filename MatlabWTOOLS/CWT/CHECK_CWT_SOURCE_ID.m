% Check CWT source identification

clear all
close all
clc

addpath ../../Core

%% Load electron count map

% Path to data
cts_path = '../../../../Data/CYGX3_BIN005/';

% File name
cts_fname = 'CYGX3_BIN005_POINTING_SIM_ISO6_GAL06.cts';

% Get file name from GUI
% [cts_fname,cmap_path] = uigetfile('*.cts', 'Load FITS file', cts_path);

% Load data and build x/y grid
[cts,cts_x,cts_y] = loadcts(fullfile(cts_path, cts_fname));
cts_sz = size(cts);

%% Sources

sx = [];
sy = [];
fid = fopen('SOURCES.txt');
cur_line = fgetl(fid);
while ischar(cur_line)
 
    cur_line = cur_line(7:25);
    
    cur_data = sscanf(cur_line, '%f %f');
    
    sx = [sx cur_data(1)];
    sy = [sy cur_data(2)];
    
    cur_line = fgetl(fid);
end
fclose(fid)

src_x = interp1(1:length(cts_x), cts_x, sx);
src_y = interp1(1:length(cts_y), cts_y, sy);

%% Plot

close all

% fh = figure('Name', 'Count map', 'NumberTitle', 'Off');
% colormap(gray)
% imagesc(cts_x, cts_y, cts)
% set(gca, 'XDir', 'reverse', 'YDir', 'normal')
% colorbar
% axis equal
% xlabel('l [deg]')
% ylabel('b [deg]')

fh = figure('Name', 'Count map', 'NumberTitle', 'Off');
colormap(gray)
imagesc(cts_x, cts_y, cts)
hold on
plot(src_x, src_y, 'go', 'LineWidth', 2)
set(gca, 'XDir', 'reverse', 'YDir', 'normal')
colorbar
axis equal
xlabel('l [deg]')
ylabel('b [deg]')
