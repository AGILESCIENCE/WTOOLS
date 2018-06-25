function w = pulse(n)

% Build pulsation vector

n2 = floor((n-1)/2);
w  = 2*pi/n*[0:n2  (n2-n+1):-1];