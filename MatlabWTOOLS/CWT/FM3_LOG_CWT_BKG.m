% Characterize background of FM3

clear all
close all
clc

addpath ../Core

%% Load gamma count map

% Path to data
cts_path = '../../Data/FM3/BKGISO5';

% Read file list
cts_file_list = dir(cts_path);

%% Wavelet transform

% Wavelet FT
csi_hat = @(wx,wy) 2*pi*(wx.^2+wy.^2).*exp(-wx.^2/2-wy.^2/2); 
csi_c   = 4*pi^3;

% Build scales
s = scale('diadic', 0.5, 8, 3);

cts_sz = []; 
count = 1;
for i = 1 : length(cts_file_list)
   
    [~,name,ext] = fileparts(cts_file_list(i).name);
    
    if strcmp(ext, '.fts')
        
        fprintf('process %s\n', [name ext]);
        
        cts = loadcts(fullfile(cts_path, cts_file_list(i).name));
        
        % Build pulsation plane
        if isempty(cts_sz);
        
            cts_sz = size(cts);
            [wx,wy] = pulse2(cts_sz(1),cts_sz(2));
            
            cts_w = NaN*ones(cts_sz(1), cts_sz(2), length(s), 100);
            
        end
        
        % Remove mean
        cts = cts - mean(cts(:));
        
        % Precompute data FT
        cts_hat = fft2(cts);
        
        % Compute CWT coeffs
        for j = 1:length(s)
    
            % Scale wavelet function
            cur_csi_hat = s(j)*csi_hat(s(j)*wx, s(j)*wy);
    
            % Compute coeffs
            cts_w(:,:,j,count) = ifft2(cts_hat.*conj(cur_csi_hat)/s(j));
        
        end
        
        count = count + 1;
        
    end
    
end

%% Get mean and std deviation of background coeffs

cts_w_mean = NaN*ones(cts_sz(1), cts_sz(2), length(s));
cts_w_std  = NaN*ones(cts_sz(1), cts_sz(2), length(s));

th_sig = 5;

for i = 1 : cts_sz(1)
    
    fprintf('stat row %d\n', i);
    
    for j = 1 : cts_sz(2)
        for k = 1 : length(s)
            
            coeff = cts_w(i,j,k,:);
            
            cts_w_mean(i,j,k) = mean(coeff(:));
            cts_w_std(i,j,k)  = std(coeff(:));
            
            % fprintf('[%3d,%3d] mean %f std %f\n', i, j, cts_w_mean(i,j,k), cts_w_std(i,j,k));
            
        end
    end
end

%% Save

% save('fm3_cwt_bkg.mat', 'cts_w_mean', 'cts_w_std')
