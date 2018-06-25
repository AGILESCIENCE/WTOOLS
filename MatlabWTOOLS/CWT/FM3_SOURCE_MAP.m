% Load counts and overlap sources

clear all
clc

addpath ../Core

%% Load electron count map

% Path to data
data_path = '../../Data/FM3/';

% File name
cts_fname = 'FM3.119_ASDCe_I0023_B01.cts.fts';

% Get file name from GUI
% [cts_fname,cts_path] = uigetfile('*.cts', 'Load FITS file', '../../Data/CYGX3_BIN005/');

% Load data and build x/y grid
[cts,cts_x,cts_y] = loadcts(fullfile(data_path, cts_fname));
cts_sz = size(cts);

cts_mean = mean(cts(:));

cts = cts - cts_mean;

%% Load source list

list_fname = 'list100.multi';

fid = fopen(fullfile(data_path, list_fname));

data = textscan(fid, '%f %f %f %f %f %f %s');

list_x    = data{2};
list_y    = data{3};
list_name = data{7};

fclose(fid);

list_fname = 'listA00.multi';

fid = fopen(fullfile(data_path, list_fname));

data = textscan(fid, '%f %f %f %f %f %f %s %f');

list_x    = [list_x ; data{2}];
list_y    = [list_y ; data{3}];
list_name = [list_name ; data{7}];

fclose(fid);

clear data

%% Save

save('fm3_source_list', 'list_x', 'list_y', 'list_name') 

%% Plot

close all

fh = figure('Name', 'Count map', 'NumberTitle', 'Off');
colormap(jet(256))
imagesc(cts_x, cts_y, cts)
set(gca, 'XDir', 'reverse', 'YDir', 'normal')
colorbar
hold on

plot(list_x, list_y, 'rs')


axis equal
xlabel('b [deg]')
ylabel('l [deg]')
