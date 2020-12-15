

function y = Gauss_0(z,N)
  
    I_0 = 0.5; 
    
        for n = 1:1:N
            b = (z./N)*n;
            a = (z./N)*(n-1);
            
            I_0 = I_0 + (((b-a)/2).*(f(g(-1,a,b))+f(g(1,a,b))));
            
        end
        y=I_0;
        
end   
    
