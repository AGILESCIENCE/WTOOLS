function [wx,wy] = pulse2(varargin)

% Build pulsation plane

if nargin == 1
   
    wx = pulse(varargin{1}(2));
    wy = pulse(varargin{1}(1));
    
elseif nargin == 2
    
    wx = pulse(varargin{2});
    wy = pulse(varargin{1});
    
else
    error('pulse2 : wrong input number');
end

[wx,wy] = meshgrid(wx,wy);