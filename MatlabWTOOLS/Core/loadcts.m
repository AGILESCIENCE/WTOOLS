function [cts,cts_x,cts_y,cts_info] = loadcts(fname)

% Load CTS FITS data

% Load data
cts_info = fitsinfo(fname);
cts = fitsread(fname);

% Build x/y grid
for i = 1 : size(cts_info.PrimaryData.Keywords, 1)
   
   switch cts_info.PrimaryData.Keywords{i,1} 
       
       case 'CRVAL1'
           x0 = cts_info.PrimaryData.Keywords{i,2};
           
       case 'CRVAL2'
           y0 = cts_info.PrimaryData.Keywords{i,2};
           
       case 'CDELT1'
           dx = cts_info.PrimaryData.Keywords{i,2};
           
       case 'CDELT2'
           dy = cts_info.PrimaryData.Keywords{i,2};
           
   end
    
end

cts_sz = size(cts); 

cts_x = dx*(-cts_sz(2)/2:cts_sz(2)/2) + x0;
cts_y = dy*(-cts_sz(1)/2:cts_sz(1)/2) + y0;