% FM3 data decompression

clear all
clc

addpath ../Core

%% Read files and check if unzip is needed 

% Path to data
file_path = '../../Data/FM3/BKGISO5';

% List files in the directory
file_list = dir(file_path);

for i = 1 : length(file_list)
   
    [pathstr,name,ext] = fileparts(file_list(i).name); 
    
    if strcmp(ext, '.gz')
       fprintf('unzip %s to %s\n', [name ext], [name ext '.fts']) 
       gunzip(fullfile(file_path, file_list(i).name), file_path)
       movefile(fullfile(file_path, [name ext]), fullfile(file_path, [name '.fts']))
    end
    
end

return

%% 

file_list = dir(file_path);

for i = 1 : length(file_list)
   
    [pathstr,name,ext] = fileparts(file_list(i).name); 
    
    if strcmp(ext, '.fts')
        movefile(fullfile(file_path, [name ext]), fullfile(file_path, name))
    end
end

%% 

file_list = dir(file_path);

for i = 1 : length(file_list)
   
    [pathstr,name,ext] = fileparts(file_list(i).name); 
    
    if strcmp(ext, '.cts')
        movefile(fullfile(file_path, [name ext]), fullfile(file_path, [name ext '.fts']))
    end
end