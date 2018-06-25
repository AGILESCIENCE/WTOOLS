function plotwt(src,evnt,x,y,w,s,lx,ly,lname)

    global n x_lim y_lim

    switch evnt.Character
        case {'+','uparrow'} 
            n = n + 1;
            if n > size(w,3)
                n = size(w,3);
            end
            
            
        case {'-','downarrow'}
            n = n - 1;
            if n < 1
                n = 1;
            end
    end
    
    if isempty(n)
        n = 1;
    end
    
    % set(src, 'Name', sprintf('[%3d] scale %10.4f : coeff. min %10.4e max %10.4e\n', n, s(n), min(min(w(:,:,n))), max(max(w(:,:,n)))));
    
    clc
    
    imagesc(x, y, w(:,:,n))
    set(gca, 'XDir', 'reverse', 'YDir', 'normal')
    if ~isempty(x_lim)
        set(gca, 'XLim', x_lim)
    end
    if ~isempty(y_lim)
        set(gca, 'YLim', y_lim)
    end
    colorbar
    
    axis equal
    
    hold on
    plot(lx, ly, 'rs')
    
    title(sprintf('[%d] scale %10.4f : coeff. min %10.4e max %10.4e', n, s(n), min(min(w(:,:,n))), max(max(w(:,:,n)))), 'FontWeight', 'bold')
    
    xlabel('l')
    ylabel('b')
    
end