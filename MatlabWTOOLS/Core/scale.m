function s = scale(type,a,b,c,d)

% Build scales for the wavelet transform

if nargin < 5
    d = 2;
end

if strcmpi(type, 'linear')          % Linear scale
    % a = min scale
    % b = max scale
    % c = number of scales
    s = linspace(a, b, c);
else                                % Logaritmic
    % 2^a = min scale
    % 2^b = max scale
    % c = number of voices
    s = d.^((a*c:b*c)/c);
end


