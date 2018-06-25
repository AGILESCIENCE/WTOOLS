function data = loadmtx(fname)

% Load MTX file

fid = fopen(fname, 'r');

% Read header
type = fread(fid, 1, 'uchar');
row  = fread(fid, 1, 'uint32');
col  = fread(fid, 1, 'uint32');
pln  = fread(fid, 1, 'uint32');

% Defines the file data type
switch type
    case 4
        data_type = 'float32';
    case 5
        data_type = 'float64'; 
    otherwise
        error('loadmtx : data type not supported');
end

% Allocate memory
data = NaN*ones(row,col,pln);

% Read data
for k = 1 : pln
   for i = 1 : row
       data(i,:,k) = fread(fid, col, data_type);
   end
end

fclose(fid);