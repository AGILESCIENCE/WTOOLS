% Test scales

clear all
close all
clc


s = scale('diadic', -1, 3, 3)


n = 13;
val_min = 2^-1;
val_max = 2^3;
b = 2;

val_min = log(val_min)/log(b);
val_max = log(val_max)/log(b);

step = (val_max - val_min)/(n-1);

s1 = b.^(val_min + (0:n-1)*step)
