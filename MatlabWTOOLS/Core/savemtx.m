function [] = savemtx(fname,data,data_type)

% Save MTX file

row = size(data,1);
col = size(data,2);
pln = size(data,3);

switch data_type
    case 'float32'
        type = 4;
    case 'float64'
        type = 5;
    otherwise
        error('savemtx : data type not supported');
end


fid = fopen(fname, 'w');

% Write header
fwrite(fid, type, 'uchar');
fwrite(fid, row, 'uint32');
fwrite(fid, col, 'uint32');
fwrite(fid, pln, 'uint32');

% Write data
for k = 1 : pln
    for i = 1 : row
        fwrite(fid, data(i,:,k), data_type); 
    end
end

fclose(fid);