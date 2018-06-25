%%

i = 10

cur_d = cts_w(:,:,i);
    
% Get MET threshold
[cur_hist_data,cur_hist_bin] = hist(cur_d(:), 128);

met(cur_hist_data, cur_hist_bin);