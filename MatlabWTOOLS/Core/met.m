function [t,h] = met(hist_data, hist_bin)

% Compute max entropy threshold

% Remove zero probability data
keep_id   = find(hist_data > 0);
hist_bin  = hist_bin(keep_id);
hist_data = hist_data(keep_id);

% Compute data probability
p = hist_data/(sum(hist_data)*(hist_bin(2)-hist_bin(1)));

% Compute classes entropy
sz = length(hist_data);
h  = zeros(1,sz);
for i = 1 : sz
    
    % First class
    id1     = 1:i;
    p1      = sum(p(id1));
    h1      = -sum((p(id1)/p1).*log(p(id1)/p1)/log(2));
    
    % Second class
    id2     = (i+1):sz;
    p2      = sum(p(id2));
    h2      = -sum((p(id2)/p2).*log(p(id2)/p2)/log(2));
    
    % Interclass entropy
    h(i)    = h1 + h2;
    
end

% Select threshold using max entropy
[dummy,i] = max(h);
t = hist_bin(i);