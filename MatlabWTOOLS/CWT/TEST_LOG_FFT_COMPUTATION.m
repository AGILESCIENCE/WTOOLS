% LoG FFT computation

clear all
close all
clc

addpath ../../Core

% Wavelet FT
csi_hat = @(wx,wy,sx,sy) (wx.^2+wy.^2).*exp(-sx^2*wx.^2/2-sy^2*wy.^2/2); 
csi_c   = 1;

sx = 1;
sy = 1;

[wx,wy] = pulse2(2,2);

wx

wy

fhat = csi_hat(wx, wy, sx, sy);

fhat